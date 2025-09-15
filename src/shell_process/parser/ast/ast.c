/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 23:01:38 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	cmd_accepts_stop(t_token_type t)
{
	return (t == T_AND || t == T_OR || t == T_PIPE || t == T_SEPARATOR
		|| t == T_PAREN_CLOSE || t == T_END);
}

static t_ast	*parse_command(t_token **tokens)
{
	t_ast	*cmd;
	t_token	*tok;
	int		have_any;

	cmd = new_ast_node(NODE_CMD);
	if (!cmd)
		return (NULL);
	have_any = 0;
	tok = *tokens;
	while (tok && !cmd_accepts_stop(tok->type))
	{
		if (tok->type == T_WORD || tok->type == T_WILDCARD)
		{
			if (!add_arg(cmd, tok->value, tok->can_expand))
				return (free_ast(cmd), NULL);
			*tokens = tok->next;
			tok = *tokens;
			have_any = 1;
			continue ;
		}
		if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_APPEND || tok->type == T_HEREDOC)
		{
			tok = parse_redirection(cmd, tok);
			if (!tok)
				return (free_ast(cmd), NULL);
			*tokens = tok;
			have_any = 1;
			continue ;
		}
		break ;
	}
	if (!have_any)
		return (free_ast(cmd), NULL);
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
