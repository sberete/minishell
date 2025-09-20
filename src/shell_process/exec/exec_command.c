/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:06:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:22:04 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_reset_cmd(t_exec *ex, t_ast *cmd)
{
	if (cmd)
		ex->redirs = cmd->redirs;
	else
		ex->redirs = NULL;
}

static int	exec_maybe_parent_builtin(t_ast *n, t_data *data)
{
	char	**av;
	int		rc;

	if (!n || n->type != NODE_CMD)
		return (-1);
	if (!(n->argv && n->argv[0] && is_builtin(n->argv[0]) && n->redirs == NULL))
		return (-1);
	av = expand_argv_dup(n, data);
	rc = 1;
	if (av)
	{
		rc = run_builtin_argv(data, av);
		free_tab(av);
	}
	data->last_exit = rc;
	g_exit_status = rc;
	return (rc);
}

static int	exec_parent_wait(pid_t pid, t_data *data)
{
	int	s;
	int	rc;

	signals_setup_parent_execwait();
	if (waitpid(pid, &s, 0) < 0)
	{
		err_sys_label("waitpid");
		rc = 1;
		data->last_exit = rc;
		return (rc);
	}
	update_last_exit_from_wait(data, s);
	rc = data->last_exit;
	g_exit_status = rc;
	return (rc);
}

int	exec_cmd_node(t_ast *n, t_data *data)
{
	int		rc;
	pid_t	pid;
	t_exec	ex;
	int		maybe;

	if (!n || n->type != NODE_CMD)
		return (1);
	maybe = exec_maybe_parent_builtin(n, data);
	if (maybe >= 0)
		return (maybe);
	exec_init(&ex, data);
	exec_reset_cmd(&ex, n);
	pid = fork();
	if (pid < 0)
	{
		err_sys_label("fork");
		return (1);
	}
	if (pid == 0)
	{
		signals_setup_child();
		exec_cmd_in_child(n, &ex);
	}
	rc = exec_parent_wait(pid, data);
	return (rc);
}
