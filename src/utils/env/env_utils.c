/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 22:10:01 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*env_new(char *key, char *value)
{
	t_env	*n;

	n = malloc(sizeof(t_env));
	if (!n)
		return (NULL);
	if (key == NULL)
		n->key = ft_strdup("");
	else
		n->key = ft_strdup(key);
	if (!n->key)
	{
		free(n);
		return (NULL);
	}
	if (value == NULL)
		value = "";
	n->value = ft_strdup(value);
	if (!n->value)
	{
		free(n->key);
		free(n);
		return (NULL);
	}
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

size_t	env_list_size(t_env *lst)
{
	size_t	n;

	n = 0;
	while (lst)
	{
		n++;
		lst = lst->next;
	}
	return (n);
}
