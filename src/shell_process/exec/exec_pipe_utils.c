/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:31:53 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:49:28 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execp_spawn_all(t_exec *ex, t_ast **nodes)
{
	size_t	i;

	i = 0;
	while (i < ex->len)
	{
		if (spawn_and_setup_one(i, ex, nodes) != 0)
			return (1);
		i++;
	}
	return (0);
}

void	exec_close_prev_read(t_exec *ex)
{
	if (ex->prev_read != -1)
	{
		close(ex->prev_read);
		ex->prev_read = -1;
	}
}

size_t	flatten_pipeline(t_ast *n, t_ast **nodes)
{
	size_t	len;

	len = 0;
	while (n && n->type == NODE_PIPE && len < 255)
	{
		nodes[len++] = n->left;
		n = n->right;
	}
	nodes[len++] = n;
	return (len);
}

int	open_next_pipe(t_exec *ex, size_t i)
{
	if (i < ex->len - 1)
	{
		if (pipe(ex->pipe_fd) != 0)
		{
			err_sys_label("pipe");
			return (1);
		}
	}
	else
	{
		ex->pipe_fd[0] = -1;
		ex->pipe_fd[1] = -1;
	}
	return (0);
}

void	child_dup_io(t_exec *ex, size_t i)
{
	if (ex->prev_read != -1)
	{
		if (dup2(ex->prev_read, STDIN_FILENO) < 0)
		{
			err_sys_label("dup2 stdin");
			exit(1);
		}
	}
	if (i < ex->len - 1)
	{
		close(ex->pipe_fd[0]);
		if (dup2(ex->pipe_fd[1], STDOUT_FILENO) < 0)
		{
			err_sys_label("dup2 stdout");
			exit(1);
		}
	}
	if (ex->prev_read != -1)
		close(ex->prev_read);
	if (ex->pipe_fd[1] != -1)
		close(ex->pipe_fd[1]);
}
