/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:50:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 00:52:14 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_tokens(t_token *tokens)
{
	t_token	*tmp;

	tmp = tokens;
	printf("Tokens extraits :\n");
	while (tmp)
	{
		printf("Token: %-10s | Type: %d | to_expand: %d\n",
			tmp->value,
			tmp->type,
			tmp->to_expand);
		tmp = tmp->next;
	}
}

