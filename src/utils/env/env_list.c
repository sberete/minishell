/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 22:23:32 by sberete          ###   ########.fr       */
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
