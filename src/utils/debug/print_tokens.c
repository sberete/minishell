/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:50:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 17:37:28 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*token_type_name(t_token_type t)
{
	if (t == T_WORD)
		return ("WORD");
	if (t == T_REDIR_IN)
		return ("<");
	if (t == T_REDIR_OUT)
		return (">");
	if (t == T_APPEND)
		return (">>");
	if (t == T_HEREDOC)
		return ("<<");
	if (t == T_AND)
		return ("&&");
	if (t == T_OR)
		return ("||");
	if (t == T_PIPE)
		return ("|");
	if (t == T_SEPARATOR)
		return (";");
	if (t == T_PAREN_OPEN)
		return ("(");
	if (t == T_PAREN_CLOSE)
		return (")");
	if (t == T_WILDCARD)
		return ("*");
	return ("UNKNOWN");
}

void	print_tokens(t_token *t)
{
	char	*val;
	char	*type;
	char	*exp;

	printf("Tokens extraits :\n");
	while (t)
	{
		val = t->value;
		if (val == NULL)
			val = "(null)";
		type = token_type_name(t->type);
		exp = "false";
		if (t->can_expand)
			exp = "true";
		printf("Token: %-12s | Type: %-10s | can_expand: %s\n", val, type, exp);
		t = t->next;
	}
}
