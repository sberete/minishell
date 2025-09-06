/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_token.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:47:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/06 16:57:10 by sxrimu           ###   ########.fr       */
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

void    free_token_list(t_token **head)
{
    t_token *n;
    t_token *next;

    if (!head || !*head)          // ← garde-fou
        return;
    n = *head;
    while (n)
    {
        next = n->next;           // ← prends next AVANT de free
        free_token_node(n);
        n = next;
    }
    *head = NULL;                  // ← remet bien à NULL
}
