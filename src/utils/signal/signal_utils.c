/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 16:50:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 22:25:53 by sberete          ###   ########.fr       */
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

void	update_exit_from_wait(int status)
{
	int	sig;

	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		g_exit_status = 128 + sig;
		if (sig == SIGINT)
			write(1, "\n", 1);
		else if (sig == SIGQUIT)
			write(1, "Quit (core dumped)\n", 19);
	}
	else if (WIFEXITED(status))
		g_exit_status = WEXITSTATUS(status);
}
