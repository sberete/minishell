/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:39 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 02:01:40 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_inc_shlvl(t_env **lst)
{
	char	*cur;
	long	lvl;
	size_t	i;
	int		rc;
	char	*newv;

	if (!lst)
		return (1);
	cur = env_get(*lst, "SHLVL");
	if (!cur || cur[0] == '\0')
		return (env_set(lst, "SHLVL", "1", 1) != 0);
	i = 0;
	if (cur[i] == '+' || cur[i] == '-')
		i++;
	while (cur[i])
	{
		if (!ft_isdigit((unsigned char)cur[i]))
			break ;
		i++;
	}
	lvl = ft_atoi(cur);
	if (lvl < 0 || cur[i] != '\0')
		lvl = 0;
	if (lvl > 1000)
		lvl = 0;
	lvl += 1;
	newv = ft_itoa((int)lvl);
	if (!newv)
		return (1);
	rc = env_set(lst, "SHLVL", newv, 1);
	free(newv);
	return (rc != 0);
}
