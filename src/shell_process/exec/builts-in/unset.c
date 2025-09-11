/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:23:56 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>

int ft_unset(char **argv, t_data *data)
{
	int i;
	int status;

	status = 0;
	i = 1;
	while (argv[i])
	{
		if (!id_is_valid(argv[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			status = 1;
		}
		else
		{
			env_remove(&data->env, argv[i]);
		}
		i++;
	}
	return status;
}
