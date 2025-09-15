/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:17:46 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	to_status(const char *s)
{
	long long	nb;

	nb = ft_atoi(s);
	return ((int)((unsigned char)nb));
}

int	builtin_exit(t_data *d, char **av)
{
	int	status;

	(void)d;
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (!av || !av[1])
		exit(0);
	if (!builtin_is_numeric(av[1]))
	{
		builtin_err_arg("exit", av[1], "numeric argument required");
		exit(2);
	}
	if (av[2])
	{
		builtin_err("exit", "too many arguments");
		return (1);
	}
	status = to_status(av[1]);
	exit(status);
}
