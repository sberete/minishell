/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 17:05:12 by sberete           #+#    #+#             */
/*   Updated: 2025/09/04 17:06:05 by sberete          ###   ########.fr       */
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

void	env_clear(t_env **lst)
{
	t_env	*n;
	t_env	*tmp;

	if (!lst || !*lst)
		return ;
	n = *lst;
	while (n)
	{
		tmp = n->next;
		env_free_one(n);
		n = tmp;
	}
	*lst = NULL;
}
