/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 01:48:49 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_exit_status = 0;
// int	event(void)
// {
// 	return (EXIT_SUCCESS);
// }

int	main(int argc, char **argv, char **env)
{
	t_data	data;
	int		rc;

	data = data_init(argc, argv, env);
	while (1)
	{
		signals_setup_parent();
		rc = read_input(&data);
		if (rc == 0)
		{
			free_data(&data);
			break ;
		}
		if (rc == 2 || shell_process(&data) == 1)
		{
			free_data_tmp(&data);
			continue ;
		}
		free_data_tmp(&data);
		if (data.last_exit == 128 + SIGINT)
			write(2, "\n", 1);
		if (data.last_exit == 128 + SIGQUIT)
			write(2, "Quit core dumped\n", 17);
	}
	return (data.last_exit);
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/
