#include "minishell.h"
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int	wait_status_to_code(int s)
{
	if (WIFEXITED(s))
		return (WEXITSTATUS(s));
	if (WIFSIGNALED(s))
		return (128 + WTERMSIG(s));
	return (1);
}

int	wait_and_get_status(pid_t pid)
{
	int	s;

	if (waitpid(pid, &s, 0) < 0)
	{
		fprintf(stderr, "minishell: waitpid: ");
		perror(NULL);
		return (1);
	}
	return (wait_status_to_code(s));
}
int	run_subshell(t_ast *sub, t_data *data)
{
	pid_t	pid;
	int		st;

	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		signals_setup_child();
		st = exec_ast(sub, data);
		_exit(st);
	}
	signals_setup_parent_execwait();
	return (wait_and_get_status(pid));
}