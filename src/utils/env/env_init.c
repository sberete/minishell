/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:39 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 22:59:16 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_digits(char *s)
{
	size_t	i;

	if (!s || s[0] == '\0')
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (s[i] == '\0')
		return (0);
	while (s[i] && ft_isdigit((unsigned char)s[i]))
		i++;
	if (s[i] == '\0')
		return (1);
	return (0);
}

static long	normalize_shlvl(char *cur)
{
	long	lvl;

	if (!is_digits(cur))
		return (0);
	lvl = ft_atoi(cur);
	if (lvl < 0 || lvl > 1000)
		return (0);
	return (lvl);
}

static int	set_shlvl_value(t_env **lst, long lvl)
{
	char	*newv;
	int		rc;

	newv = ft_itoa((int)lvl);
	if (!newv)
		return (1);
	rc = env_set(lst, "SHLVL", newv, 1);
	free(newv);
	if (rc != 0)
		return (1);
	return (0);
}

int	env_inc_shlvl(t_env **lst)
{
	char	*cur;
	long	lvl;

	if (!lst)
		return (1);
	cur = env_get(*lst, "SHLVL");
	if (!cur || cur[0] == '\0')
		return (set_shlvl_value(lst, 1));
	lvl = normalize_shlvl(cur);
	return (set_shlvl_value(lst, lvl + 1));
}
