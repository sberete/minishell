#include "minishell.h"
#include <errno.h>

/* helpers locaux */
static int	exec_cmd_node(t_ast *cmd, t_data *data, int in_fd, int out_fd, int in_pipe);
static int	exec_pipeline_node(t_ast *n, t_data *data);
static int	exec_andor_node(t_ast *n, t_data *data);
static int	exec_seq_node(t_ast *n, t_data *data);
static int	exec_group_node(t_ast *n, t_data *data);

/* 1/5 : exécute une commande simple (builtin parent si pas de pipe) */
static int	exec_cmd_node(t_ast *cmd, t_data *data, int in_fd, int out_fd, int in_pipe)
{
	char	**argv;
	int		sin;
	int		sout;
	pid_t	pid;

	if (!cmd)
		return (1);
	if (expand_redirs_inplace(cmd->redirs, data) != 0)
		return (1);
	argv = expand_argv_full(cmd, data);
	if (!argv)
		return (1);
	if (!argv[0] && !cmd->redirs)
	{
		free_tab(argv);
		return (0);
	}
	if (!in_pipe && argv[0] && is_builtin(argv[0]))
	{
		if (save_stdio(&sin, &sout) != 0)
			return (free_tab(argv), 1);
		if (redirect_io(in_fd, out_fd) != 0 || apply_redirs(cmd->redirs) != 0)
		{
			restore_stdio(sin, sout);
			return (free_tab(argv), 1);
		}
		data->last_exit = exec_builtin(data, cmd);
		restore_stdio(sin, sout);
		free_tab(argv);
		return (data->last_exit);
	}
	pid = fork();
	if (pid < 0)
		return (free_tab(argv), 1);
	if (pid == 0)
	{
		signals_setup_child();
		if (redirect_io(in_fd, out_fd) != 0 || apply_redirs(cmd->redirs) != 0)
			_exit(1);
		if (argv[0] && is_builtin(argv[0]))
			_exit(exec_builtin(data, cmd));
		if (!argv[0])
			_exit(0);
		/* externe */
		{
			char	*path;
			char	**envp;

			path = find_cmd_path(argv[0], data->env);
			if (!path)
				_exit(127);
			envp = env_list_to_envp(data->env);
			if (!envp)
				_exit(1);
			execve(path, argv, envp);
			free(path);
			free_tab(envp);
			_exit(127);
		}
	}
	signals_setup_parent_execwait();
	free_tab(argv);
	return (wait_and_get_status(pid));
}

/* 2/5 : exécute un pipeline binaire (gauche | droite), itératif */
static int	exec_pipeline_node(t_ast *n, t_data *data)
{
	t_ast	*nodes[256];
	int		count;
	int		i;
	int		prev_in;
	int		pfd[2];
	pid_t	last_pid;
	pid_t	pid;

	count = 0;
	while (n && n->type == NODE_PIPE && count < 255)
	{
		nodes[count] = n->left;
		count++;
		n = n->right;
	}
	nodes[count] = n;
	count++;
	prev_in = -1;
	last_pid = -1;
	i = 0;
	while (i < count)
	{
		if (i < count - 1)
		{
			if (pipe(pfd) != 0)
			{
				if (prev_in >= 0)
					close(prev_in);
				return (1);
			}
		}
		pid = fork();
		if (pid < 0)
		{
			if (prev_in >= 0)
				close(prev_in);
			if (i < count - 1)
			{
				close(pfd[0]);
				close(pfd[1]);
			}
			return (1);
		}
		if (pid == 0)
		{
			signals_setup_child();
			if (i < count - 1 && dup2(pfd[1], STDOUT_FILENO) < 0)
				_exit(1);
			if (i < count - 1)
				close(pfd[0]);
			if (i < count - 1)
				close(pfd[1]);
			if (prev_in >= 0 && dup2(prev_in, STDIN_FILENO) < 0)
				_exit(1);
			if (prev_in >= 0)
				close(prev_in);
			if (nodes[i]->type == NODE_CMD)
				_exit(exec_cmd_node(nodes[i], data, -1, -1, 1));
			else if (nodes[i]->type == NODE_GROUP)
				_exit(run_subshell(nodes[i]->child, data));
			else
				_exit(exec_ast(nodes[i], data));
		}
		if (prev_in >= 0)
			close(prev_in);
		if (i < count - 1)
		{
			close(pfd[1]);
			prev_in = pfd[0];
		}
		last_pid = pid;
		i++;
	}
	{
		int		status;
		pid_t	w;

		status = 0;
		while (1)
		{
			w = waitpid(-1, NULL, 0);
			if (w < 0)
				break ;
			if (w == last_pid)
				status = wait_and_get_status(w);
		}
		return (status);
	}
}

/* 3/5 : exécute AND/OR */
static int	exec_andor_node(t_ast *n, t_data *data)
{
	int	ls;

	if (!n || !n->left || !n->right)
		return (1);
	ls = exec_ast(n->left, data);
	if (n->type == NODE_AND)
	{
		if (ls == 0)
			return (exec_ast(n->right, data));
		return (ls);
	}
	if (n->type == NODE_OR)
	{
		if (ls != 0)
			return (exec_ast(n->right, data));
		return (ls);
	}
	return (1);
}

/* 4/5 : séquence ';' */
static int	exec_seq_node(t_ast *n, t_data *data)
{
	int	st;

	if (!n)
		return (1);
	st = exec_ast(n->left, data);
	(void)st;
	return (exec_ast(n->right, data));
}

/* 5/5 : groupe '( ... )' dans un sous-shell */
static int	exec_group_node(t_ast *n, t_data *data)
{
	if (!n)
		return (1);
	return (run_subshell(n->child, data));
}

/* PUBLIC: wrapper top-level (prépare heredocs une seule fois) */
int	exec_ast(t_ast *node, t_data *data)
{
	static int	depth = 0;
	int			st;

	if (!node)
		return (0);
	if (depth == 0)
	{
		if (prepare_all_heredocs(node, data) != 0)
			return (130);
	}
	depth++;
	if (node->type == NODE_CMD)
		st = exec_cmd_node(node, data, -1, -1, 0);
	else if (node->type == NODE_PIPE)
		st = exec_pipeline_node(node, data);
	else if (node->type == NODE_AND || node->type == NODE_OR)
		st = exec_andor_node(node, data);
	else if (node->type == NODE_SEQ)
		st = exec_seq_node(node, data);
	else if (node->type == NODE_GROUP)
		st = exec_group_node(node, data);
	else
		st = 1;
	depth--;
	data->last_exit = st;
	return (st);
}
