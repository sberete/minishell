/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/06 16:42:47 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **env)
{
	t_data	data;

	data = data_init(argc, argv, env);
	while (1)
	{
		if (!read_input(&data))
		{
			free_data(&data);
			break;
		}
		if (shell_process(&data) == 1)
		{
			free_data_tmp(&data);
			continue ;
		}
		free_data_tmp(&data);
	}
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/
