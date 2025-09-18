/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 06:12:38 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 06:12:39 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ps_dispatch(t_token **pt, t_token_type *prev, int *paren)
{
	t_token_type	tt;

	tt = (*pt)->type;
	if (tt == T_SEPARATOR)
		return (ps_handle_separator(pt, prev));
	if (tt == T_PAREN_OPEN)
		return (ps_handle_paren_open(pt, prev, paren));
	if (tt == T_PAREN_CLOSE)
		return (ps_handle_paren_close(pt, prev, paren));
	if (is_binop_tok(tt))
		return (ps_handle_binop(pt, prev));
	if (is_redir_tok(tt))
		return (ps_handle_redir(pt, prev));
	if (tt == T_WORD)
		return (ps_handle_word(pt, prev));
	return (ps_error("unexpected token"));
}

int	parser_syntax(t_token *t)
{
	int				paren;
	t_token_type	prev;

	paren = 0;
	prev = T_SEPARATOR;
	while (t)
	{
		if (t->type == T_UNKNOWN)
			return (ps_error("unknown token"));
		if (t->type == T_END)
			break ;
		if (!ps_dispatch(&t, &prev, &paren))
			return (0);
	}
	if (paren != 0)
		return (ps_error("unclosed '('"));
	if (prev == T_SEPARATOR || prev == T_PAREN_OPEN)
		return (ps_error("unexpected end of line"));
	if (is_binop_tok(prev) || is_redir_tok(prev))
		return (ps_error("unexpected end of line"));
	return (1);
}

t_ast	*parse_entry(t_data *data)
{
	t_token	*cur;
	t_ast	*root;

	if (!data || !data->tokens)
		return (NULL);
	if (!parser_syntax(data->tokens))
		return (NULL);
	cur = data->tokens;
	root = parse_sequence(&cur);
	if (!root)
		return (NULL);
	if (cur != NULL)
	{
		free_ast(root);
		print_syntax_error("trailing tokens");
		set_exit_status(2);
		return (NULL);
	}
	return (root);
}
