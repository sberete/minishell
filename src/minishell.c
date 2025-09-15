/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 22:59:29 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_exit_status = 0;

int	main(int argc, char **argv, char **env)
{
	t_data	data;
	int		rc;

	data = data_init(argc, argv, env);
	signals_setup_parent();
	while (1)
	{
		rc = read_input(&data);
		// sync_exit_status(&data);
		if (rc == 0)
		{
			free_data(&data);
			break ;
		}
		if (rc == 2)
		{
			free_data_tmp(&data);
			continue ;
		}
		if (shell_process(&data) == 1)
		{
			free_data_tmp(&data);
			continue ;
		}
		free_data_tmp(&data);
	}
	return (data.last_exit);
}
/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/
