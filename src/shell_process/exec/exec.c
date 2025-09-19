#include "minishell.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

/* --- helpers erreur std --- */

static void	err_simple(char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("\n", 2);
}

static void	err_cmd_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
}

static void	err_sys_label(char *label)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)label, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd((char *)strerror(errno), 2);
	ft_putstr_fd("\n", 2);
}
/* libère les buffers temporaires de l’enfant et quitte */
static void	child_free_and_exit(char **argv, char **envp, char *path, int code)
{
	if (argv)
		free_tab(argv);
	if (envp)
		free_tab(envp);
	if (path)
		free(path);
	_exit(code);
}

// /* 1/7 : builtins parent (sans redirections) */
// static int	is_parent_builtin(char *name)
// {
// 	return (name && (str_eq(name, "cd") || str_eq(name, "export")
// 			|| str_eq(name, "unset") || str_eq(name, "exit")));
// }

/* 2/7 : exécuter une CMD dans l’enfant (pipeline/subshell inclus) */
static void	exec_cmd_in_child(t_ast *cmd, t_data *data)
{
	char	**argv;
	char	*path;
	char	**envp;
	int		st;

	if (expand_redirs_inplace(cmd->redirs, data) != 0)
	{
		err_simple("redirection: expansion failed");
		_exit(1);
	}
	if (apply_redirs(cmd->redirs) != 0)
		_exit(1);
	argv = expand_argv_dup(cmd, data);
	if (!argv)
		_exit(1);
	if (!argv[0])
		child_free_and_exit(argv, NULL, NULL, 0);
	if (is_builtin(argv[0]))
	{
		st = run_builtin_argv(data, argv);
		child_free_and_exit(argv, NULL, NULL, st);
	}
	path = find_cmd_path(argv[0], data->env);
	if (!path)
	{
		err_cmd_not_found(argv[0]);
		child_free_and_exit(argv, NULL, NULL, 127);
	}
	envp = env_list_to_envp(data->env);
	if (!envp)
		child_free_and_exit(argv, NULL, path, 1);
	execve(path, argv, envp);
	/* si execve échoue */
	err_sys_label(path);
	if (errno == ENOENT)
		child_free_and_exit(argv, envp, path, 127);
	else
		child_free_and_exit(argv, envp, path, 126);
}

/* 3/7 : exécute une commande simple (fork si nécessaire) */
int	exec_cmd_node(t_ast *n, t_data *data)
{
	char	**av;
	int		rc;
	pid_t	pid;
	int		s;

	if (!n || n->type != NODE_CMD)
		return (1);
	/* Builtins parent (sans redirections) */
	if (n->argv && n->argv[0] && is_builtin(n->argv[0]) && n->redirs == NULL)
	{
		av = expand_argv_dup(n, data);
		rc = 1;
		if (av)
		{
			rc = run_builtin_argv(data, av);
			free_tab(av); /* <<< IMPORTANT */
		}
		data->last_exit = rc;
		return (rc);
	}
	/* Sinon: fork et exécuter dans l’enfant (gère aussi redir seule) */
	pid = fork();
	if (pid < 0)
	{
		err_sys_label("fork");
		return (1);
	}
	if (pid == 0)
	{
		signals_setup_child();
		exec_cmd_in_child(n, data);
		_exit(127);
	}
	signals_setup_parent_execwait();
	if (waitpid(pid, &s, 0) < 0)
	{
		err_sys_label("waitpid");
		rc = 1;
		data->last_exit = rc;
	}
	else
	{
		update_last_exit_from_wait(data, s); /* 128+sig ou exit code */
		// print_signal_msg_if_any(data);
		/* "\n" ou "Quit (core dumped)\n" */
		rc = data->last_exit;
		g_exit_status = rc; /* sync globale pour le prompt */
	}
	return (rc);
}

/* 4/7 : exécute un pipeline (gauche | droite), itératif */
static int	exec_pipeline_node(t_ast *n, t_data *data)
{
	t_ast	*nodes[256];
	int		count;
	int		i;
	int		prev_in;
	int		pfd[2];
	pid_t	last_pid;
	pid_t	pid;
	int		s;
	int		rc;
	pid_t	w;

	count = 0;
	while (n && n->type == NODE_PIPE && count < 255)
	{
		nodes[count++] = n->left;
		n = n->right;
	}
	nodes[count++] = n;
	prev_in = -1;
	last_pid = -1;
	i = 0;
	while (i < count)
	{
		if (i < count - 1 && pipe(pfd) != 0)
		{
			err_sys_label("pipe");
			if (prev_in != -1)
				close(prev_in);
			return (1);
		}
		pid = fork();
		if (pid < 0)
		{
			err_sys_label("fork");
			if (prev_in != -1)
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
			if (prev_in != -1)
			{
				if (dup2(prev_in, STDIN_FILENO) < 0)
				{
					err_sys_label("dup2 stdin");
					_exit(1);
				}
				close(prev_in);
			}
			if (i < count - 1)
			{
				close(pfd[0]);
				if (dup2(pfd[1], STDOUT_FILENO) < 0)
				{
					err_sys_label("dup2 stdout");
					_exit(1);
				}
				close(pfd[1]);
			}
			if (nodes[i]->type == NODE_CMD)
				exec_cmd_in_child(nodes[i], data);
			else if (nodes[i]->type == NODE_GROUP)
				_exit(run_subshell(nodes[i]->child, data));
			else
				_exit(exec_ast(nodes[i], data));
		}
		if (prev_in != -1)
			close(prev_in);
		if (i < count - 1)
		{
			close(pfd[1]);
			prev_in = pfd[0];
		}
		last_pid = pid;
		i++;
	}
	rc = 0;
	while ((w = waitpid(-1, &s, 0)) > 0)
	{
		if (w == last_pid)
		{
			update_last_exit_from_wait(data, s);
			// print_signal_msg_if_any(data);
				/* message UNE seule fois (dernier) */
			rc = data->last_exit;
			g_exit_status = rc; /* sync globale */
		}
	}
	if (w < 0 && errno != ECHILD)
		err_sys_label("waitpid");
	data->last_exit = rc;
	return (rc);
}

/* 5/7 : exécute AND/OR */
static int	exec_and_or_node(t_ast *n, t_data *d)
{
	int	st;

	st = exec_ast(n->left, d);
	if (n->type == NODE_AND)
	{
		if (st == 0)
			st = exec_ast(n->right, d);
	}
	else
	{
		if (st != 0)
			st = exec_ast(n->right, d);
	}
	d->last_exit = st;
	return (st);
}

/* 6/7 : séquence ';' */
static int	exec_seq_node(t_ast *n, t_data *data)
{
	if (!n)
		return (1);
	(void)exec_ast(n->left, data);
	return (exec_ast(n->right, data));
}

/* 7/7 : groupe '( ... )' dans un sous-shell */
static int	exec_group_node(t_ast *n, t_data *data)
{
	int	st;

	if (!n)
		return (1);
	st = run_subshell(n->child, data);
	data->last_exit = st;
	return (st);
}

/* PUBLIC: wrapper top-level (prépare heredocs une seule fois) */
int	exec_ast(t_ast *node, t_data *data)
{
	static int	depth;
	int			st;

	if (!node)
		return (0);
	if (depth == 0)
	{
		if (prepare_all_heredocs(node, data) != 0)
		{
			err_simple("heredoc: aborted");
			return (130);
		}
	}
	depth++;
	if (node->type == NODE_CMD)
		st = exec_cmd_node(node, data);
	else if (node->type == NODE_PIPE)
		st = exec_pipeline_node(node, data);
	else if (node->type == NODE_AND || node->type == NODE_OR)
		st = exec_and_or_node(node, data);
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
