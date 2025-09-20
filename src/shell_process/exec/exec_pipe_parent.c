/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_parent.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 23:20:44 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:20:50 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	wait_set_last_if_final(t_exec *ex, size_t i, int s, int *rc)
{
	if (ex->pid[i] != ex->pid[ex->len - 1])
		return ;
	update_last_exit_from_wait(ex->data, s);
	print_signal_msg_if_any(ex->data);
	*rc = ex->data->last_exit;
	g_exit_status = *rc;
}

int	wait_all_children(t_exec *ex)
{
	size_t	i;
	int		s;
	int		rc;
	pid_t	w;

	i = 0;
	rc = 0;
	while (i < ex->len)
	{
		w = waitpid(ex->pid[i], &s, 0);
		if (w < 0)
		{
			err_sys_label("waitpid");
			if (rc == 0)
				rc = 1;
		}
		else
			wait_set_last_if_final(ex, i, s, &rc);
		i++;
	}
	return (rc);
}
