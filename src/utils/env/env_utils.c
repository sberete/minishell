/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:19 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 02:01:20 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	id_is_valid(char *s)
{
	size_t	i;

	if (!s || !*s)
		return (0);
	if (!(ft_isalpha(s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

int	parse_assignment(char *s, char **key, char **val, int *append)
{
	char	*p;
	size_t		kl;

	if (!s || !key || !val || !append)
		return (-1);
	*append = 0;
	p = ft_strchr(s, '=');
	if (!p)
	{
		if (!id_is_valid(s))
			return (-1);
		*key = ft_strdup(s);
		if (!*key)
			return (-1);
		*val = ft_strdup("");
		if (!*val)
			return (free(*key), -1);
		return (0);
	}
	if (p > s && *(p - 1) == '+')
	{
		*append = 1;
		kl = (size_t)(p - s - 1);
	}
	else
		kl = (size_t)(p - s);
	*key = (char *)malloc(kl + 1);
	if (!*key)
		return (-1);
	ft_memcpy(*key, s, kl);
	(*key)[kl] = '\0';
	if (!id_is_valid(*key))
		return (free(*key), -1);
	*val = ft_strdup(p + 1);
	if (!*val)
		return (free(*key), -1);
	return (0);
}

char	*env_get(t_env *lst, char *key)
{
	while (lst)
	{
		if (ft_strcmp(lst->key, key) == 0)
			return (lst->value);
		lst = lst->next;
	}
	return (NULL);
}

int	env_set(t_env **lst, char *key, char *value, int overwrite)
{
	t_env	*cur;

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
		return (0);
	free(cur->value);
	cur->value = ft_strdup(value);
	if (!cur->value)
		return (-1);
	return (0);
}

int	env_append(t_env **lst, char *key, char *suffix)
{
	t_env	*cur;
	char	*joined;

	if (!lst || !key || !suffix)
		return (-1);
	cur = env_find(*lst, key);
	if (!cur)
		return (env_set(lst, key, suffix, 1));
	joined = ft_strjoin(cur->value ? cur->value : "", suffix);
	if (!joined)
		return (-1);
	free(cur->value);
	cur->value = joined;
	return (0);
}
