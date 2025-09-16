/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 16:50:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 19:37:50 by sxrimu           ###   ########.fr       */
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
