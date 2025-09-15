/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 02:11:26 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*env_new(char *key, char *value)
{
	t_env	*n;

	n = (t_env *)malloc(sizeof(t_env));
	if (!n)
		return (NULL);
	n->key = ft_strdup(key ? key : "");
	if (!n->key)
		return (free(n), NULL);
	if (!value)
		value = "";
	n->value = ft_strdup(value);
	if (!n->value)
		return (free(n->key), free(n), NULL);
	n->next = NULL;
	return (n);
}

void	env_add_back(t_env **lst, t_env *node)
{
	t_env	*cur;

	if (!lst || !node)
		return ;
	if (!*lst)
	{
		*lst = node;
		return ;
	}
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = node;
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

int	env_remove(t_env **lst, char *key)
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
