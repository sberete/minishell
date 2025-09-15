/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:25:10 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_unset(t_data *data, char **argv)
{
	int	i;
	int	status;

	if (!argv || !argv[1])
		return (0);
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (!unset_is_valid_name(argv[i]))
		{
			builtin_err_arg("unset", argv[i], "not a valid identifier");
			status = 1;
		}
		else if (unset_remove_key(&data->env, argv[i]) != 0)
			status = 1;
		i++;
	}
	return (status);
}
