/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:48:57 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 22:53:47 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	hd_emit_line(int wfd, char *out)
{
	if (!out)
		return (-1);
	if (fd_write_all(wfd, out, ft_strlen(out)) != 0)
	{
		free(out);
		return (-1);
	}
	if (fd_write_all(wfd, "\n", 1) != 0)
	{
		free(out);
		return (-1);
	}
	free(out);
	return (0);
}

static int	hd_process_line(int wfd, char *line, int do_expand, t_exec *ex)
{
	char	*out;

	if (do_expand)
		out = ms_expand_vars(line, ex->data);
	else
		out = ft_strdup(line);
	free(line);
	return (hd_emit_line(wfd, out));
}

static int	heredoc_loop(int wfd, char *delim, int do_expand, t_exec *ex)
{
	char	*line;

	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			break ;
		}
		if (hd_process_line(wfd, line, do_expand, ex) != 0)
			return (-1);
	}
	return (0);
}

int	run_one_heredoc_child(t_redir *r, t_exec *ex)
{
	char	*plain;
	int		do_expand;
	int		rc;

	signals_setup_heredoc_child();
	do_expand = 0;
	if (r->delim_can_expand)
		do_expand = 1;
	plain = unquote_all(r->delim);
	if (!plain)
		return (1);
	rc = heredoc_loop(ex->fd_heredoc[1], plain, do_expand, ex);
	free(plain);
	if (rc != 0)
		return (1);
	return (0);
}
