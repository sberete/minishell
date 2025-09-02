/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_token.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:47:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/01 19:37:14 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_token_node(t_token *n)
{
	if (!n)
		return ;
	if (n->value)
		free(n->value);
	free(n);
}

void	free_token_list(t_token **head)
{
	t_token	*tmp;

	while (*head)
	{
		tmp = (*head)->next;
		free((*head)->value);
		free(*head);
		*head = tmp;
	}
	*head = NULL;
}
