/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:18:55 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:34:37 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	fd_write_all(int fd, void *buf, size_t n)
{
	char	*p;
	size_t	i;
	ssize_t	w;

	p = (char *)buf;
	i = 0;
	while (i < n)
	{
		w = write(fd, p + i, n - i);
		if (w < 0)
		{
			if (errno == EINTR)
				continue ;
			return (-1);
		}
		i += (size_t)w;
	}
	return (0);
}

int	open_heredoc_pipe(t_exec *ex)
{
	if (pipe(ex->fd_heredoc) != 0)
		return (1);
	return (0);
}

int	heredoc_wait_handle(pid_t pid, int rfd, t_data *d)
{
	int		st;
	pid_t	w;

	w = waitpid(pid, &st, 0);
	if (w < 0)
	{
		fd_close_quiet(&rfd);
		return (1);
	}
	if (WIFSIGNALED(st))
	{
		fd_close_quiet(&rfd);
		d->last_exit = 130;
		return (1);
	}
	if (WIFEXITED(st) && WEXITSTATUS(st) != 0)
	{
		fd_close_quiet(&rfd);
		return (1);
	}
	return (0);
}
