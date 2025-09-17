/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/17 16:20:25 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_exit_status = 0;

int	main(int argc, char **argv, char **env)
{
	t_data	data;
	int		rc;

	data = data_init(argc, argv, env);
	while (1)
	{
		/* remets les handlers du prompt à chaque tour */
		signals_setup_parent();

		rc = read_input(&data);

		if (rc == 0)                 /* ^D / EOF → quitter proprement */
		{
			free_data(&data);
			break ;
		}

		/* ici seulement on traite la ligne */
		if (rc == 2 || shell_process(&data) == 1)
		{
			free_data_tmp(&data);
			continue ;
		}

		/* si tu veux synchroniser g_exit_status → data->last_exit */
		sync_exit_status(&data);
		free_data_tmp(&data);
	}
	return (data.last_exit);
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/
