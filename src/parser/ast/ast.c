/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/03 20:01:59 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// static int	count_cmd_args(t_token *tok)
// {
// 	int	count;

// 	count = 0;
// 	while (tok && tok->type != T_AND && tok->type != T_OR && tok->type != T_PIPE
// 		&& tok->type != T_SEPARATOR && tok->type != T_PAREN_CLOSE
// 		&& tok->type != T_END)
// 	{
// 		if (tok->type == T_WORD)
// 			count++;
// 		else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
// 			|| tok->type == T_APPEND || tok->type == T_HEREDOC)
// 		{
// 			tok = tok->next;                // skip opérateur
// 			if (tok && tok->type == T_WORD) // skip filename
// 				tok = tok->next;
// 			continue ;
// 		}
// 		tok = tok->next;
// 	}
// 	return (count);
// }

t_ast	*parse_command(t_token **tokens)
{
	t_token	*tok;
	t_ast	*node;
	int		argc;
	t_token	*cur;
	int		i;
	t_token	*next;

	tok = *tokens;
	node = new_ast_node(NODE_CMD);
	argc = 0;
	if (!node)
		return (NULL);
	// 1) Compter args en sautant redirs
	cur = tok;
	while (cur && cur->type != T_PIPE && cur->type != T_AND && cur->type != T_OR
		&& cur->type != T_SEPARATOR && cur->type != T_PAREN_CLOSE)
	{
		if (cur->type == T_REDIR_IN || cur->type == T_REDIR_OUT
			|| cur->type == T_APPEND || cur->type == T_HEREDOC)
		{
			if (!cur->next || cur->next->type != T_WORD)
				return (free_ast(node), NULL); // "redir sans fichier"
			cur = cur->next->next;             // skip op + filename
			continue ;
		}
		if (cur->type == T_WORD)
			argc++;
		cur = cur->next;
	}
	node->argv = (char **)malloc(sizeof(char *) * (argc + 1));
	if (!node->argv)
		return (free(node), NULL);
	// 2) Remplir argv & redirs
	i = 0;
	while (tok && tok != cur)
	{
		if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_APPEND || tok->type == T_HEREDOC)
		{
			next = parse_redirection(node, tok);
			if (next == tok) // <- parse_redirection a signalé une erreur
				return (free_ast(node), NULL);
			tok = next;
			continue ;
		}
		if (tok->type == T_WORD)
			node->argv[i++] = ft_strdup(tok->value);
		tok = tok->next;
	}
	node->argv[i] = NULL;
	*tokens = cur;
	if (i == 0)
	{
		free_ast(node);
		return (NULL);
	} // pas de commande réelle
	return (node);
}

t_ast	*parse_group(t_token **tokens)
{
	t_ast	*child;
	t_ast	*group;

	if (!*tokens || (*tokens)->type != T_PAREN_OPEN)
		return (parse_command(tokens));
	*tokens = (*tokens)->next; // skip '('
	child = parse_sequence(tokens);
	if (!*tokens || (*tokens)->type != T_PAREN_CLOSE)
	{
		printf("Erreur: parenthèse non fermée\n");
		return (NULL);
	}
	*tokens = (*tokens)->next; // skip ')'
	group = new_ast_node(NODE_GROUP);
	group->child = child;
	return (group);
}

t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_token	*tok;
	t_ast	*right;
	t_ast	*pipe_node;

	left = parse_group(tokens);
	tok = *tokens;
	while (tok && tok->type == T_PIPE)
	{
		*tokens = tok->next;
		right = parse_group(tokens);
		pipe_node = new_ast_node(NODE_PIPE);
		pipe_node->left = left;
		pipe_node->right = right;
		left = pipe_node;
		tok = *tokens;
	}
	return (left);
}

t_ast	*parse_and_or(t_token **tokens)
{
	t_ast		*left;
	t_token		*tok;
	t_node_type	type;
	t_ast		*right;
	t_ast		*node;

	left = parse_pipeline(tokens);
	tok = *tokens;
	while (tok && (tok->type == T_AND || tok->type == T_OR))
	{
		type = (tok->type == T_AND) ? NODE_AND : NODE_OR;
		*tokens = tok->next;
		right = parse_pipeline(tokens);
		node = new_ast_node(type);
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
	t_token	*tok;
	t_ast	*right;
	t_ast	*seq_node;

	left = parse_and_or(tokens);
	tok = *tokens;
	while (tok && tok->type == T_SEPARATOR) // ";"
	{
		*tokens = tok->next;
		right = parse_and_or(tokens);
		seq_node = new_ast_node(NODE_SEQ);
		seq_node->left = left;
		seq_node->right = right;
		left = seq_node;
		tok = *tokens;
	}
	return (left);
}
