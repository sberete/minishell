/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 20:00:00 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **env)
{
	t_data	data;

	data = data_init(argc, argv, env);
	while (1)
	{
		data.line = read_input(&data);
		if (!data.line)
		{
		free_data(&data);
			break;
		}
		if (shell_process(&data) == 1)
		{
			free_data(&data);
			continue ;
		}
		free_data(&data);
	}
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/
