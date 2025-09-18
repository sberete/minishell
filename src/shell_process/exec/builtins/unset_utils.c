/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 01:35:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:00:26 by sberete          ###   ########.fr       */
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

static int	env_remove(t_env **lst, char *key)
{
	t_env	*cur;
	t_env	*prev;

	if (!lst || !*lst || !key)
		return (-1);
	prev = NULL;
	cur = *lst;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*lst = cur->next;
			free(cur->key);
			free(cur->value);
			free(cur);
			return (0);
		}
		prev = cur;
		cur = cur->next;
	}
	return (-1);
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
