#include "minishell.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/* 1/7 : builtins parent (sans redirections) */
static int	is_parent_builtin(char *name)
{
	return (name && (str_eq(name, "cd") || str_eq(name, "export")
			|| str_eq(name, "unset") || str_eq(name, "exit")));
}

/* 2/7 : exécuter une CMD dans l’enfant (pipeline/subshell inclus) */
static void	exec_cmd_in_child(t_ast *cmd, t_data *data)
{
	char	**argv;
	char	*path;
	char	**envp;
	int		st;

	/* Redirs d’abord, pour gérer “> f” même sans commande */
	if (expand_redirs_inplace(cmd->redirs, data) != 0)
	{
		fprintf(stderr, "minishell: redirection: expansion failed\n");
		_exit(1);
	}
	if (apply_redirs(cmd->redirs) != 0)
		_exit(1);
	/* argv */
	argv = expand_argv_dup(cmd, data);
	if (!argv || !argv[0])
		_exit(0);
	/* builtin dans l’enfant */
	if (is_builtin(argv[0]))
	{
		st = run_builtin_argv(data, argv);
		_exit(st);
	}
	/* externe */
	path = find_cmd_path(argv[0], data->env);
	if (!path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
		_exit(127);
	}
	envp = env_list_to_envp(data->env);
	if (!envp)
	{
		fprintf(stderr, "minishell: env: allocation failed\n");
		_exit(1);
	}
	execve(path, argv, envp);
	/* si execve échoue */
	fprintf(stderr, "minishell: %s: ", path);
	perror(NULL);
	_exit(errno == ENOENT ? 127 : 126);
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
	if (n->argv && n->argv[0] && is_parent_builtin(n->argv[0])
		&& n->redirs == NULL)
	{
		av = expand_argv_dup(n, data);
		rc = av ? run_builtin_argv(data, av) : 1;
		data->last_exit = rc;
		return (rc);
	}
	/* Sinon: fork et exécuter dans l’enfant (gère aussi redir seule) */
	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "minishell: fork: ");
		perror(NULL);
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
		fprintf(stderr, "minishell: waitpid: ");
		perror(NULL);
		rc = 1;
	}
	else
		rc = wait_status_to_code(s);
	data->last_exit = rc;
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
			fprintf(stderr, "minishell: pipe: ");
			perror(NULL);
			if (prev_in != -1)
				close(prev_in);
			return (1);
		}
		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "minishell: fork: ");
			perror(NULL);
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
					fprintf(stderr, "minishell: dup2 stdin: ");
					perror(NULL);
					_exit(1);
				}
				close(prev_in);
			}
			if (i < count - 1)
			{
				close(pfd[0]);
				if (dup2(pfd[1], STDOUT_FILENO) < 0)
				{
					fprintf(stderr, "minishell: dup2 stdout: ");
					perror(NULL);
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
			rc = wait_status_to_code(s);
	}
	if (w < 0 && errno != ECHILD)
	{
		fprintf(stderr, "minishell: waitpid: ");
		perror(NULL);
	}
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
			fprintf(stderr, "minishell: heredoc: aborted\n");
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
