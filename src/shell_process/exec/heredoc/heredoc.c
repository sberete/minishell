/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:54:36 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:50:34 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	hd_child(t_redir *r, t_exec *ex)
{
	fd_close_quiet(&ex->fd_heredoc[0]);
	if (run_one_heredoc_child(r, ex) != 0)
	{
		fd_close_quiet(&ex->fd_heredoc[1]);
		exit(1);
	}
	fd_close_quiet(&ex->fd_heredoc[1]);
	free_data(ex->data);
	exit(0);
}

int	run_one_heredoc(t_redir *r, t_exec *ex)
{
	pid_t	pid;

	if (open_heredoc_pipe(ex) != 0)
		return (1);
	pid = fork();
	if (pid < 0)
	{
		fd_close_pair(ex->fd_heredoc);
		return (1);
	}
	if (pid == 0)
		hd_child(r, ex);
	signals_setup_heredoc_parent();
	fd_close_quiet(&ex->fd_heredoc[1]);
	if (heredoc_wait_handle(pid, ex->fd_heredoc[0], ex->data) != 0)
		return (1);
	r->fd = ex->fd_heredoc[0];
	r->type = REDIR_IN;
	if (r->delim)
	{
		free(r->delim);
		r->delim = NULL;
	}
	ex->fd_heredoc[0] = -1;
	return (0);
}

int	run_heredocs_for_redirs_exec(t_redir *r, t_exec *ex)
{
	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			if (run_one_heredoc(r, ex) != 0)
				return (1);
		}
		r = r->next;
	}
	return (0);
}

int	prepare_all_heredocs_exec(t_ast *n, t_exec *ex)
{
	if (!n)
		return (0);
	if (n->type == NODE_CMD)
	{
		if (run_heredocs_for_redirs_exec(n->redirs, ex) != 0)
			return (1);
	}
	if (prepare_all_heredocs_exec(n->left, ex) != 0)
		return (1);
	if (prepare_all_heredocs_exec(n->right, ex) != 0)
		return (1);
	if (prepare_all_heredocs_exec(n->child, ex) != 0)
		return (1);
	return (0);
}

int	prepare_all_heredocs(t_ast *n, t_data *d)
{
	t_exec	ex;

	exec_init(&ex, d);
	return (prepare_all_heredocs_exec(n, &ex));
}
