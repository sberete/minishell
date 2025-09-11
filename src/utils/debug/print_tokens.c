/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:50:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/09 18:41:49 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_tokens(t_token *t)
{
	printf("Tokens extraits :\n");
	while (t)
	{
		const char *val = t->value;
		const char *exp = "false";

		if (val == NULL)
			val = "(null)";
		if (t->can_expand)
			exp = "true";

		printf("Token: %-10s | Type: %d | can_expand: %s\n",
			val, (int)t->type, exp);
		t = t->next;
	}
}
