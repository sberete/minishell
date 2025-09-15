/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 01:35:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:32:43 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset_is_valid_name(char *s)
{
	size_t	i;

	if (!s || !*s)
		return (0);
	if (s[0] == '=' || !(ft_isalpha(s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (s[i] == '=')
			return (0);
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

int	unset_remove_key(t_env **env, char *name)
{
	int	rc;

	if (!env || !name)
		return (1);
	rc = env_remove(env, name);
	if (rc == 0 || rc == -1)
		return (0);
	return (1);
}
