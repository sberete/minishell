/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 01:26:12 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:26:57 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	str_eq(char *a, char *b)
{
	if (!a || !b)
		return (0);
	return (ft_strcmp(a, b) == 0);
}

int	builtin_argc(char **argv)
{
	int	n;

	if (!argv)
		return (0);
	n = 0;
	while (argv[n])
		n++;
	return (n);
}

void	builtin_err(char *cmd, char *msg)
{
	char	*c;
	char	*m;

	c = cmd;
	if (!c)
		c = "(null)";
	m = msg;
	if (!m)
		m = "";
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)c, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd((char *)m, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

void	builtin_err_arg(char *cmd, char *arg, char *msg)
{
	char	*c;
	char	*a;
	char	*m;

	c = cmd;
	if (!c)
		c = "(null)";
	a = arg;
	if (!a)
		a = "(null)";
	m = msg;
	if (!m)
		m = "";
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)c, STDERR_FILENO);
	ft_putstr_fd(": `", STDERR_FILENO);
	ft_putstr_fd((char *)a, STDERR_FILENO);
	ft_putstr_fd("': ", STDERR_FILENO);
	ft_putstr_fd((char *)m, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

int	builtin_is_numeric(char *s)
{
	size_t	i;

	if (!s || !*s)
		return (0);
	if (s[0] == '+' || s[0] == '-')
		s++;
	i = 0;
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}
