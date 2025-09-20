/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pids.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 23:45:21 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:45:22 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_pids_init(t_exec *ex, size_t len)
{
	ex->pid = NULL;
	ex->len = len;
	if (len == 0)
		return (0);
	ex->pid = (pid_t *)malloc(sizeof(pid_t) * len);
	if (!ex->pid)
		return (1);
	return (0);
}

void	exec_pids_free(t_exec *ex)
{
	if (ex->pid)
		free(ex->pid);
	ex->pid = NULL;
	ex->len = 0;
}
