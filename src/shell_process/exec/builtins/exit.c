/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 03:10:12 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 03:35:12 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/history.h>

/* Bash: le status est un octet (mod 256) */
static int	to_status(const char *s)
{
	long long	nb;

	nb = ft_atoi(s);
	return ((int)((unsigned char)nb));
}

static void	exit_now(t_data *d, int status)
{
	if (d)
		free_data(d);
	rl_clear_history();
	exit(status);
}

int	builtin_exit(t_data *d, char **av)
{
	int	status;

	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (!av || !av[1])
	{
		exit_now(d, 0);
		return (0);
	}
	if (!builtin_is_numeric(av[1]))
	{
		builtin_err_arg("exit", av[1], "numeric argument required");
		exit_now(d, 2);
		return (2);
	}
	if (av[2])
	{
		builtin_err("exit", "too many arguments");
		if (d)
			d->last_exit = 1;
		return (1);
	}
	status = to_status(av[1]);
	exit_now(d, status);
	return (status);
}
