/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 01:26:12 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 18:05:22 by sberete          ###   ########.fr       */
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

t_env	*env_find(t_env *lst, char *key)
{
	while (lst)
	{
		if (ft_strcmp(lst->key, key) == 0)
			return (lst);
		lst = lst->next;
	}
	return (NULL);
}

int	env_set(t_env **lst, char *key, char *value, int overwrite)
{
	t_env	*cur;
	char	*dup;

	if (!lst || !key || !value)
		return (-1);
	cur = env_find(*lst, key);
	if (!cur)
	{
		cur = env_new(key, value);
		if (!cur)
			return (-1);
		env_add_back(lst, cur);
		return (0);
	}
	if (!overwrite)
		return (1);
	dup = ft_strdup(value);
	if (!dup)
		return (-1);
	free(cur->value);
	cur->value = dup;
	return (0);
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
