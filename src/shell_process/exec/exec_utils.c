#include "minishell.h"
#include <sys/wait.h>
#include <errno.h>

/* 1/5 */
int	save_stdio(int *saved_in, int *saved_out)
{
	if (!saved_in || !saved_out)
		return (1);
	*saved_in = dup(STDIN_FILENO);
	if (*saved_in < 0)
		return (1);
	*saved_out = dup(STDOUT_FILENO);
	if (*saved_out < 0)
	{
		close(*saved_in);
		return (1);
	}
	return (0);
}

/* 2/5 */
void	restore_stdio(int saved_in, int saved_out)
{
	if (saved_in >= 0)
	{
		dup2(saved_in, STDIN_FILENO);
		close(saved_in);
	}
	if (saved_out >= 0)
	{
		dup2(saved_out, STDOUT_FILENO);
		close(saved_out);
	}
}

/* 3/5 : 0..255 normalisé ; 128+sig si signalé */
int	wait_and_get_status(pid_t pid)
{
	int	st;

	if (waitpid(pid, &st, 0) < 0)
		return (1);
	if (WIFEXITED(st))
		return (WEXITSTATUS(st));
	if (WIFSIGNALED(st))
		return (128 + WTERMSIG(st));
	return (1);
}

/* 4/5 : fork + exec_ast(sub) dans un sous-shell, renvoie le status */
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
