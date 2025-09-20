/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 16:50:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 18:04:33 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_exit_status(int code)
{
	g_exit_status = code;
}

void	sync_exit_status(t_data *data)
{
	data->last_exit = (int)g_exit_status;
}

void	print_signal_msg_if_any(t_data *data)
{
	if (data->last_exit == 128 + SIGINT)
		write(2, "\n", 1);
	else if (data->last_exit == 128 + SIGQUIT)
		ft_putstr_fd("Quit (core dumped)\n", 2);
}

void	update_last_exit_from_wait(t_data *data, int status)
{
	if (WIFSIGNALED(status))
		data->last_exit = 128 + WTERMSIG(status);
	else
		data->last_exit = WEXITSTATUS(status);
}
