/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_handlers_paren.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 06:12:23 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 06:12:24 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ps_handle_paren_open(t_token **pt, t_token_type *prev, int *paren)
{
	t_token	*next;

	*paren += 1;
	*prev = T_PAREN_OPEN;
	*pt = (*pt)->next;
	next = *pt;
	if (!next)
		return (ps_error("empty group"));
	if (next->type == T_PAREN_CLOSE)
		return (ps_error("empty group"));
	if (next->type == T_SEPARATOR)
		return (ps_error("empty group"));
	if (is_binop_tok(next->type))
		return (ps_error("empty group"));
	return (1);
}

int	ps_handle_paren_close(t_token **pt, t_token_type *prev, int *paren)
{
	if (*paren == 0)
		return (ps_error("unexpected ')'"));
	if (*prev == T_SEPARATOR)
		return (ps_error("unexpected ')'"));
	if (*prev == T_PAREN_OPEN)
		return (ps_error("unexpected ')'"));
	if (is_binop_tok(*prev))
		return (ps_error("unexpected ')'"));
	if (is_redir_tok(*prev))
		return (ps_error("unexpected ')'"));
	*paren -= 1;
	*prev = T_PAREN_CLOSE;
	*pt = (*pt)->next;
	return (1);
}
