/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_token.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:47:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 22:22:30 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_token_node(t_token *n)
{
	if (!n)
		return ;
	free(n->value);
	free(n);
}

void	free_token_list(t_token **head)
{
	t_token	*n;
	t_token	*next;

	if (!head || !*head)
		return ;
	n = *head;
	while (n)
	{
		next = n->next;
		free_token_node(n);
		n = next;
	}
	*head = NULL;
}
