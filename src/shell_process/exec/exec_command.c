#include "minishell.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static int	is_parent_builtin(char *name)
{
	return (name
		&& (str_eq(name, "cd")
			|| str_eq(name, "export")
			|| str_eq(name, "unset")
			|| str_eq(name, "exit")));
}

void	child_exec_cmd(t_ast *cmd, t_exec *x)
{
	int	st;

	/* 1) redirs d’abord (gère “> f” sans argv) */
	if (expand_redirs_inplace(x->redirs, x->data) != 0)
	{
		fprintf(stderr, "minishell: redirection: expansion failed\n");
		_exit(1);
	}
	if (apply_redirs(x->redirs) != 0)
		_exit(1);

	/* 2) argv */
	x->argv = expand_argv_dup(cmd, x->data);
	if (!x->argv || !x->argv[0])
		_exit(0);

	/* 3) builtin enfant */
	if (is_builtin(x->argv[0]))
	{
		st = run_builtin_argv(x->data, x->argv);
		exec_ctx_free_argv(x);
		exec_ctx_free_envp(x);
		exec_ctx_free_path(x);
		exec_ctx_close_pipes(x);
		_exit(st);
	}

	/* 4) externe */
	x->path = find_cmd_path(x->argv[0], x->data->env);
	if (!x->path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", x->argv[0]);
		_exit(127);
	}
	x->envp = env_list_to_envp(x->data->env);
	if (!x->envp)
	{
		fprintf(stderr, "minishell: env: allocation failed\n");
		_exit(1);
	}
	execve(x->path, x->argv, x->envp);

	/* execve a échoué */
	fprintf(stderr, "minishell: %s: ", x->path);
	perror(NULL);
	_exit(errno == ENOENT ? 127 : 126);
}

int	exec_cmd_node(t_ast *n, t_data *data)
{
	t_exec	x;
	pid_t	pid;
	int		s;
	int		rc;

	if (!n || n->type != NODE_CMD)
		return (1);
	exec_ctx_init_cmd(&x, data, n);

	/* Builtin parent (sans redirections) → pas de fork */
	if (n->argv && n->argv[0] && is_parent_builtin(n->argv[0]) && x.redirs == NULL)
	{
		x.argv = expand_argv_dup(n, data);
		if (!x.argv)
		{
			fprintf(stderr, "minishell: expand: allocation failed\n");
			rc = 1;
		}
		else
		{
			rc = run_builtin_argv(data, x.argv);
			exec_ctx_free_argv(&x);
		}
		data->last_exit = rc;
		return (rc);
	}

	/* Sinon: fork, exécuter dans l’enfant */
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
		child_exec_cmd(n, &x);
		_exit(127);
	}
	signals_setup_parent_execwait();

	if (waitpid(pid, &s, 0) < 0)
	{
		fprintf(stderr, "minishell: waitpid: ");
		perror(NULL);
		rc = 1;
	}
	else if (WIFEXITED(s))
		rc = WEXITSTATUS(s);
	else if (WIFSIGNALED(s))
		rc = 128 + WTERMSIG(s);
	else
		rc = 1;

	/* parent: nettoyages éventuels si on avait prérempli */
	exec_ctx_free_path(&x);
	exec_ctx_free_envp(&x);
	exec_ctx_free_argv(&x);
	exec_ctx_close_pipes(&x);

	data->last_exit = rc;
	return (rc);
}
