#include "minishell.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static int	setup_child_pipe_io(t_exec *x, int is_last)
{
	if (x->prev_read >= 0)
	{
		if (dup2(x->prev_read, STDIN_FILENO) < 0)
		{
			fprintf(stderr, "minishell: dup2 stdin: ");
			perror(NULL);
			return (1);
		}
		close(x->prev_read);
		x->prev_read = -1;
	}
	if (!is_last)
	{
		close(x->pipe_fd[0]);
		if (dup2(x->pipe_fd[1], STDOUT_FILENO) < 0)
		{
			fprintf(stderr, "minishell: dup2 stdout: ");
			perror(NULL);
			return (1);
		}
		close(x->pipe_fd[1]);
		x->pipe_fd[1] = -1;
	}
	return (0);
}

static int	wait_pipeline(pid_t last_pid)
{
	int		rc;
	int		s;
	pid_t	w;

	rc = 0;
	while ((w = waitpid(-1, &s, 0)) > 0)
	{
		if (w == last_pid)
		{
			if (WIFEXITED(s))
				rc = WEXITSTATUS(s);
			else if (WIFSIGNALED(s))
				rc = 128 + WTERMSIG(s);
			else
				rc = 1;
		}
	}
	if (w < 0 && errno != ECHILD)
	{
		fprintf(stderr, "minishell: waitpid: ");
		perror(NULL);
	}
	return (rc);
}

int	exec_pipeline_node(t_ast *n, t_data *data)
{
	t_ast	*nodes[256];
	int		count;
	int		i;
	t_exec	x;
	pid_t	last_pid;
	pid_t	pid;

	count = 0;
	while (n && n->type == NODE_PIPE && count < 255)
	{
		nodes[count++] = n->left;
		n = n->right;
	}
	nodes[count++] = n;

	exec_ctx_init_cmd(&x, data, NULL);
	x.prev_read = -1;
	last_pid = -1;
	i = 0;
	while (i < count)
	{
		if (i < count - 1)
		{
			if (pipe(x.pipe_fd) != 0)
			{
				fprintf(stderr, "minishell: pipe: ");
				perror(NULL);
				exec_ctx_close_pipes(&x);
				return (1);
			}
		}
		else
			x.pipe_fd[0] = x.pipe_fd[1] = -1;

		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "minishell: fork: ");
			perror(NULL);
			exec_ctx_close_pipes(&x);
			return (1);
		}
		if (pid == 0)
		{
			int	is_last = (i == count - 1);
			signals_setup_child();
			if (setup_child_pipe_io(&x, is_last) != 0)
				_exit(1);
			/* exécuter le noeud dans l’enfant */
			if (nodes[i]->type == NODE_CMD)
				child_exec_cmd(nodes[i], &x); /* la même que dans exec_command.c */
			else if (nodes[i]->type == NODE_GROUP)
				_exit(run_subshell(nodes[i]->child, data));
			else
				_exit(exec_ast(nodes[i], data));
		}
		/* parent */
		if (x.prev_read >= 0)
			close(x.prev_read);
		if (i < count - 1)
		{
			close(x.pipe_fd[1]);
			x.prev_read = x.pipe_fd[0];
			x.pipe_fd[0] = -1;
			x.pipe_fd[1] = -1;
		}
		last_pid = pid;
		i++;
	}
	/* parent: attente et nettoyage */
	exec_ctx_close_pipes(&x);
	data->last_exit = wait_pipeline(last_pid);
	return (data->last_exit);
}
