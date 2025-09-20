/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 23:11:03 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 17:20:57 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	child_exec_node(t_ast *node, t_exec *ex)
{
	if (node->type == NODE_CMD)
		exec_cmd_in_child(node, ex);
	else if (node->type == NODE_GROUP)
		exit(run_subshell(node->child, ex->data));
	else
		exit(exec_ast(node, ex->data));
}

static void	parent_after_fork(t_exec *ex, pid_t pid, size_t i)
{
	ex->pid[i] = pid;
	exec_close_prev_read(ex);
	if (i < ex->len - 1)
	{
		close(ex->pipe_fd[1]);
		ex->prev_read = ex->pipe_fd[0];
		ex->pipe_fd[0] = -1;
		ex->pipe_fd[1] = -1;
	}
	else
	{
		exec_close_pipe(ex->pipe_fd);
		exec_close_prev_read(ex);
	}
}

int	spawn_and_setup_one(size_t i, t_exec *ex, t_ast **nodes)
{
	pid_t	pid;

	if (open_next_pipe(ex, i) != 0)
		return (1);
	pid = fork();
	if (pid < 0)
	{
		err_sys_label("fork");
		exec_close_pipe(ex->pipe_fd);
		exec_close_prev_read(ex);
		return (1);
	}
	if (pid == 0)
	{
		signals_setup_child();
		child_dup_io(ex, i);
		exec_reset_cmd(ex, nodes[i]);
		child_exec_node(nodes[i], ex);
	}
	parent_after_fork(ex, pid, i);
	return (0);
}

int	pipeline_prepare(t_ast *n, t_exec *ex, t_ast **nodes, t_data *data)
{
	size_t	len;

	exec_init(ex, data);
	len = flatten_pipeline(n, nodes);
	if (exec_pids_init(ex, len) != 0)
	{
		err_sys_label("malloc pids");
		return (1);
	}
	return (0);
}

int	exec_pipeline_node(t_ast *n, t_data *data)
{
	t_exec	ex;
	t_ast	*nodes[256];
	size_t	i;
	int		rc;

	if (pipeline_prepare(n, &ex, nodes, data) != 0)
		return (1);
	i = 0;
	rc = 0;
	while (i < ex.len)
	{
		if (spawn_and_setup_one(i, &ex, nodes) != 0)
		{
			rc = 1;
			break ;
		}
		i++;
	}
	if (rc != 0)
		ex.len = i;
	if (rc == 0)
		rc = wait_all_children(&ex);
	exec_pids_free(&ex);
	ex.data->last_exit = rc;
	return (rc);
}
