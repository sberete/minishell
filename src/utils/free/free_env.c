/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 17:05:12 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 22:23:08 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	env_free_one(t_env *n)
{
	if (!n)
		return ;
	free(n->key);
	free(n->value);
	free(n);
}

void	free_env(t_env **lst)
{
	t_env	*cur;
	t_env	*nx;

	if (!lst || !*lst)
		return ;
	cur = *lst;
	while (cur)
	{
		nx = cur->next;
		env_free_one(cur);
		cur = nx;
	}
	*lst = NULL;
}
