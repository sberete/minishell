/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 01:49:37 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:27:26 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	redirect_io(int in_fd, int out_fd)
{
	if (in_fd >= 0)
	{
		if (dup2_close(in_fd, STDIN_FILENO, "dup2 stdin") != 0)
			return (1);
	}
	if (out_fd >= 0)
	{
		if (dup2_close(out_fd, STDOUT_FILENO, "dup2 stdout") != 0)
			return (1);
	}
	return (0);
}

int	apply_redirs(t_redir *r)
{
	while (r)
	{
		if (r->type == REDIR_IN || r->type == REDIR_HEREDOC)
		{
			if (redir_apply_in(r) != 0)
				return (1);
		}
		else if (r->type == REDIR_OUT || r->type == REDIR_APPEND)
		{
			if (redir_apply_out(r) != 0)
				return (1);
		}
		r = r->next;
	}
	return (0);
}

int	redirs_touch_only(t_redir *r)
{
	while (r)
	{
		if (r->type == REDIR_OUT || r->type == REDIR_APPEND)
		{
			if (redir_touch_out(r) != 0)
				return (1);
		}
		else if (r->type == REDIR_IN)
		{
			if (redir_touch_in(r) != 0)
				return (1);
		}
		else if (r->type == REDIR_HEREDOC)
			redir_discard_heredoc(r);
		r = r->next;
	}
	return (0);
}
