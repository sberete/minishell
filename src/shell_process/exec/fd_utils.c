/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 23:31:14 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 18:09:00 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	fd_close_quiet(int *fd)
{
	if (fd && *fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
}

void	fd_close_pair(int fd2[2])
{
	if (!fd2)
		return ;
	if (fd2[0] >= 0)
		close(fd2[0]);
	if (fd2[1] >= 0)
		close(fd2[1]);
	fd2[0] = -1;
	fd2[1] = -1;
}

int	dup2_close(int src, int dst, char *ctx)
{
	if (src < 0)
		return (0);
	if (dup2(src, dst) < 0)
	{
		err_sys_label(ctx);
		close(src);
		return (1);
	}
	close(src);
	return (0);
}

int	open_out_fd(t_redir *r)
{
	int	flags;

	flags = O_CREAT | O_WRONLY;
	if (r->type == REDIR_OUT)
		flags |= O_TRUNC;
	else
		flags |= O_APPEND;
	return (open(r->filename, flags, 0644));
}

void	exec_close_pipe(int pfd[2])
{
	if (pfd[0] != -1)
		close(pfd[0]);
	if (pfd[1] != -1)
		close(pfd[1]);
	pfd[0] = -1;
	pfd[1] = -1;
}
