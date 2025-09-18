/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_handlers_ops.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 06:12:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 06:12:01 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ps_handle_separator(t_token **pt, t_token_type *prev)
{
	if (*prev == T_SEPARATOR)
		return (ps_error("unexpected ';'"));
	if (*prev == T_PAREN_OPEN)
		return (ps_error("unexpected ';'"));
	if (is_binop_tok(*prev))
		return (ps_error("unexpected ';'"));
	if (is_redir_tok(*prev))
		return (ps_error("unexpected ';'"));
	*prev = T_SEPARATOR;
	*pt = (*pt)->next;
	return (1);
}

int	ps_handle_binop(t_token **pt, t_token_type *prev)
{
	t_token	*next;

	if (*prev == T_SEPARATOR)
		return (ps_error("unexpected binary operator"));
	if (*prev == T_PAREN_OPEN)
		return (ps_error("unexpected binary operator"));
	if (is_binop_tok(*prev))
		return (ps_error("unexpected binary operator"));
	if (is_redir_tok(*prev))
		return (ps_error("unexpected binary operator"));
	next = (*pt)->next;
	if (!next)
		return (ps_error("operator at end"));
	if (!can_start_cmd_tok(next->type))
		return (ps_error("operator at end"));
	*prev = (*pt)->type;
	*pt = next;
	return (1);
}

int	ps_handle_redir(t_token **pt, t_token_type *prev)
{
	t_token	*arg;

	arg = (*pt)->next;
	if (!arg)
		return (ps_error("redirection without target"));
	if (arg->type != T_WORD)
		return (ps_error("redirection without target"));
	*prev = T_WORD;
	*pt = arg->next;
	return (1);
}

int	ps_handle_word(t_token **pt, t_token_type *prev)
{
	*prev = T_WORD;
	*pt = (*pt)->next;
	return (1);
}
