/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:34:01 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:48:15 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_init(t_exec *ex, t_data *data)
{
	ex->prev_read = -1;
	ex->pipe_fd[0] = -1;
	ex->pipe_fd[1] = -1;
	ex->fd_heredoc[0] = -1;
	ex->fd_heredoc[1] = -1;
	ex->path = NULL;
	ex->argv = NULL;
	ex->envp = NULL;
	ex->pid = NULL;
	ex->len = 0;
	ex->redirs = NULL;
	ex->data = data;
}
