/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:08 by sberete           #+#    #+#             */
/*   Updated: 2025/08/25 21:55:42 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast		*parse_sequence(t_token **tokens);
t_ast		*parse_and_or(t_token **tokens);
t_ast		*parse_pipeline(t_token **tokens);
t_ast		*parse_command(t_token **tokens);
t_ast		*parse_group(t_token **tokens);

static int	count_cmd_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != T_AND && tok->type != T_OR && tok->type != T_PIPE
		&& tok->type != T_SEPARATOR && tok->type != T_PAREN_CLOSE)
	{
		if (tok->type == T_WORD)
			count++;
		else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_APPEND || tok->type == T_HEREDOC)
		{
			tok = tok->next;                // skip operator
			if (tok && tok->type == T_WORD) // skip filename
				tok = tok->next;
			continue ;
		}
		tok = tok->next;
	}
	return (count);
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

t_ast	*parse_command(t_token **tokens)
{
	t_ast	*cmd;
	t_token	*tok;
	int		argc;
	char	**argv;
	int		i;

	cmd = new_ast_node(NODE_CMD);
	tok = *tokens;
	argc = count_cmd_args(tok);
	argv = malloc(sizeof(char *) * (argc + 1));
	i = 0;
	if (!argv)
		return (NULL);
	while (tok && tok->type != T_AND && tok->type != T_OR && tok->type != T_PIPE
		&& tok->type != T_SEPARATOR && tok->type != T_PAREN_CLOSE)
	{
		if (tok->type == T_WORD)
		{
			argv[i++] = ft_strdup(tok->value);
			tok = tok->next;
		}
		else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_APPEND || tok->type == T_HEREDOC)
		{
			tok = parse_redirection(cmd, tok);
			if (!tok)
			{
				free_tab(argv);
				return (NULL);
			}
		}
		else
			tok = tok->next;
	}
	argv[i] = NULL;
	cmd->argv = argv;
	*tokens = tok;
	return (cmd);
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
