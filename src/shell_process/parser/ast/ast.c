/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 20:27:20 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_ctrl_token(t_token_type t)
{
	if (t == T_PIPE)
		return (1);
	if (t == T_AND)
		return (1);
	if (t == T_OR)
		return (1);
	if (t == T_SEPARATOR)
		return (1);
	if (t == T_PAREN_CLOSE)
		return (1);
	if (t == T_END)
		return (1);
	return (0);
}

static t_ast	*parse_command(t_token **tokp)
{
	t_ast	*cmd;
	t_token	*t;

	if (!tokp || !*tokp)
		return (NULL);
	cmd = new_ast_node(NODE_CMD);
	if (!cmd)
		return (NULL);
	t = *tokp;
	while (t && !is_ctrl_token(t->type))
	{
		if (t->type == T_WORD)
		{
			if (!add_arg(cmd, t->value, t->can_expand))
				return (free_ast(cmd), NULL);
			t = t->next;
			continue ;
		}
		if (t->type == T_REDIR_IN || t->type == T_REDIR_OUT
			|| t->type == T_APPEND || t->type == T_HEREDOC)
		{
			if (parse_redirection(cmd, &t) != 0)
				return (free_ast(cmd), NULL);
			continue ;
		}
		return (free_ast(cmd), NULL);
	}
	*tokp = t;
	return (cmd);
}

static t_ast	*parse_group(t_token **tokens)
{
	t_token	*tok;
	t_ast	*child;
	t_ast	*grp;

	tok = *tokens;
	if (!tok)
		return (NULL);
	if (tok->type != T_PAREN_OPEN)
		return (parse_command(tokens));
	*tokens = tok->next;
	child = parse_sequence(tokens);
	if (!child)
		return (NULL);
	tok = *tokens;
	if (!tok || tok->type != T_PAREN_CLOSE)
		return (free_ast(child), NULL);
	*tokens = tok->next;
	grp = new_ast_node(NODE_GROUP);
	if (!grp)
		return (free_ast(child), NULL);
	grp->child = child;
	return (grp);
}

static t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_ast	*right;
	t_token	*tok;
	t_ast	*node;

	left = parse_group(tokens);
	if (!left)
		return (NULL);
	tok = *tokens;
	while (tok && tok->type == T_PIPE)
	{
		*tokens = tok->next;
		right = parse_group(tokens);
		if (!right)
			return (free_ast(left), NULL);
		node = new_ast_node(NODE_PIPE);
		if (!node)
			return (free_ast(left), free_ast(right), NULL);
		node->left = left;
		node->right = right;
		left = node;
		tok = *tokens;
	}
	return (left);
}

static t_ast	*parse_and_or(t_token **tokens)
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
	while (tok && (tok->type == T_AND || tok->type == T_OR))
	{
		if (tok->type == T_AND)
			ntype = NODE_AND;
		else
			ntype = NODE_OR;
		*tokens = tok->next;
		right = parse_pipeline(tokens);
		if (!right)
			return (free_ast(left), NULL);
		node = new_ast_node(ntype);
		if (!node)
			return (free_ast(left), free_ast(right), NULL);
		node->left = left;
		node->right = right;
		left = node;
		tok = *tokens;
	}
	return (left);
}

t_ast	*parse_sequence(t_token **tokens)
{
	t_ast	*left;
	t_ast	*right;
	t_token	*tok;
	t_ast	*node;

	left = parse_and_or(tokens);
	if (!left)
		return (NULL);
	tok = *tokens;
	while (tok && tok->type == T_SEPARATOR)
	{
		*tokens = tok->next;
		right = parse_and_or(tokens);
		if (!right)
			return (free_ast(left), NULL);
		node = new_ast_node(NODE_SEQ);
		if (!node)
			return (free_ast(left), free_ast(right), NULL);
		node->left = left;
		node->right = right;
		left = node;
		tok = *tokens;
	}
	return (left);
}
