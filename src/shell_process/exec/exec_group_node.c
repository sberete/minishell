#include "minishell.h"

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

int	exec_group_node(t_ast *n, t_data *data)
{
	int	st;

	if (!n)
		return (1);
	st = run_subshell(n->child, data);
	data->last_exit = st;
	return (st);
}
