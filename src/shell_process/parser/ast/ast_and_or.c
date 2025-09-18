/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_and_or.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:11:50 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:22:33 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_and_or_tok(t_token *tok)
{
	if (!tok)
		return (0);
	if (tok->type == T_AND)
		return (1);
	if (tok->type == T_OR)
		return (1);
	return (0);
}

static int	set_ntype_from_tok(t_token *tok, t_node_type *ntype)
{
	if (!tok || !ntype)
		return (1);
	if (tok->type == T_AND)
	{
		*ntype = NODE_AND;
		return (0);
	}
	if (tok->type == T_OR)
	{
		*ntype = NODE_OR;
		return (0);
	}
	return (1);
}

static t_ast	*make_bin_node(t_node_type ntype, t_ast *left, t_ast *right)
{
	t_ast	*node;

	node = new_ast_node(ntype);
	if (!node)
		return (NULL);
	node->left = left;
	node->right = right;
	return (node);
}

t_ast	*parse_and_or(t_token **tokens)
{
	t_ast		*left;
	t_ast		*right;
	t_token		*tok;
	t_ast		*node;
	t_node_type	ntype;

	left = parse_pipeline(tokens);
	if (!left)
		return (NULL);
	tok = *tokens;
	while (is_and_or_tok(tok))
	{
		if (set_ntype_from_tok(tok, &ntype) != 0)
			return (free_ast(left), NULL);
		*tokens = tok->next;
		right = parse_pipeline(tokens);
		if (!right)
			return (free_ast(left), NULL);
		node = make_bin_node(ntype, left, right);
		if (!node)
			return (free_ast(left), free_ast(right), NULL);
		left = node;
		tok = *tokens;
	}
	return (left);
}
