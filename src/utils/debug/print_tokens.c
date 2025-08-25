/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:50:16 by sberete           #+#    #+#             */
/*   Updated: 2025/08/25 21:50:17 by sberete          ###   ########.fr       */
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
		printf("Token: %-10s | Type: %d\n", tmp->value, tmp->type);
		tmp = tmp->next;
	}
}
