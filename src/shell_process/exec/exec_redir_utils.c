/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 23:26:38 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 18:12:06 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	redir_apply_in(t_redir *r)
{
	int	fd;

	if (r->fd >= 0)
	{
		if (dup2_close(r->fd, STDIN_FILENO, "dup2 heredoc") != 0)
			return (1);
		r->fd = -1;
		return (0);
	}
	fd = open(r->filename, O_RDONLY);
	if (fd < 0)
		return (redir_err_open(r->filename));
	if (dup2_close(fd, STDIN_FILENO, "dup2 <") != 0)
		return (1);
	return (0);
}

int	redir_apply_out(t_redir *r)
{
	int	fd;

	fd = open_out_fd(r);
	if (fd < 0)
		return (redir_err_open(r->filename));
	if (dup2_close(fd, STDOUT_FILENO, "dup2 > / >>") != 0)
		return (1);
	return (0);
}

int	redir_touch_in(t_redir *r)
{
	int	fd;

	fd = open(r->filename, O_RDONLY);
	if (fd < 0)
		return (redir_err_open(r->filename));
	close(fd);
	printf("Test\n");
	return (0);
}

int	redir_touch_out(t_redir *r)
{
	int	fd;

	fd = open_out_fd(r);
	if (fd < 0)
		return (redir_err_open(r->filename));
	close(fd);
	return (0);
}

void	redir_discard_heredoc(t_redir *r)
{
	if (r->type == REDIR_HEREDOC && r->fd >= 0)
	{
		close(r->fd);
		r->fd = -1;
	}
}
