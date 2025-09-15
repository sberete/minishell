/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:40:14 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:17:22 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_n_flag(const char *s)
{
	size_t	i;

	if (!s || s[0] != '-' || s[1] != 'n')
		return (0);
	i = 2;
	while (s[i])
	{
		if (s[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	builtin_echo(t_data *d, char **av)
{
	int	i;
	int	no_nl;
	int	first;

	(void)d;
	i = 1;
	no_nl = 0;
	while (av[i] && is_n_flag(av[i]))
	{
		no_nl = 1;
		i++;
	}
	first = 1;
	while (av[i])
	{
		if (!first)
			ft_putstr_fd(" ", STDOUT_FILENO);
		ft_putstr_fd(av[i], STDOUT_FILENO);
		first = 0;
		i++;
	}
	if (!no_nl)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (0);
}
