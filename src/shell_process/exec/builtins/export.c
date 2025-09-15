/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 02:15:28 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_export(t_data *data, char **argv)
{
	int	i;
	int	status;
	int	rc;

	if (!argv || !argv[1])
	{
		export_print_sorted(data->env);
		return (0);
	}
	i = 1;
	status = 0;
	while (argv[i])
	{
		rc = export_set_assignment(&data->env, argv[i]);
		if (rc != 0)
		{
			builtin_err_arg("export", argv[i], "not a valid identifier");
			status = 1;
		}
		i++;
	}
	return (status);
}
