/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/07 20:23:22 by sberete          ###   ########.fr       */
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
	t_ast	*cmd;
	t_token	*scan;
	int		argc;
	int		i;
	t_token	*next;

	tok = *tokens;
	cmd = NULL;
	argc = 0;
	/* 0) Redirs en tête (<< EOF, > out, < in, >> out) */
	while (tok && (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_APPEND || tok->type == T_HEREDOC))
	{
		if (!cmd)
		{
			cmd = new_ast_node(NODE_CMD);
			if (!cmd)
				return (NULL);
		}
		next = parse_redirection(cmd, tok); /* consomme op + WORD */
		if (next == tok)                    /* ERREUR si pas d'avancement */
			return (free_ast(cmd), NULL);
		tok = next; /* AVANCER (NULL autorisé) */
	}
	/* Si redirs en tête et pas de mot ensuite → CMD vide mais valide */
	if (cmd && (!tok || tok->type != T_WORD))
	{
		*tokens = tok;
		return (cmd);
	}
	/* 1) Compter les WORD jusqu'à opérateur de niveau sup. */
	scan = tok;
	while (scan && scan->type != T_PIPE && scan->type != T_AND
		&& scan->type != T_OR && scan->type != T_SEPARATOR
		&& scan->type != T_PAREN_CLOSE)
	{
		if (scan->type == T_REDIR_IN || scan->type == T_REDIR_OUT
			|| scan->type == T_APPEND || scan->type == T_HEREDOC)
		{
			/* Valider la présence d'un WORD derrière l'opérateur */
			if (!scan->next || scan->next->type != T_WORD)
				return (free_ast(cmd), NULL);
			scan = scan->next->next; /* skip op + WORD */
			continue ;
		}
		if (scan->type == T_WORD)
			argc++;
		scan = scan->next;
	}
	/* 2) S'assurer d'avoir un noeud CMD */
	if (!cmd)
	{
		cmd = new_ast_node(NODE_CMD);
		if (!cmd)
			return (NULL);
	}
	/* 3) Allouer argv */
	cmd->argv = (char **)malloc(sizeof(char *) * (argc + 1));
	if (!cmd->argv)
		return (free_ast(cmd), NULL);
	/* 4) Remplir argv & redirs entre [tok..scan) */
	i = 0;
	while (tok && tok != scan)
	{
		if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
			|| tok->type == T_APPEND || tok->type == T_HEREDOC)
		{
			next = parse_redirection(cmd, tok);
			if (next == tok) /* ERREUR si pas d'avancement */
				return (free_ast(cmd), NULL);
			tok = next; /* AVANCER (NULL autorisé) */
			continue ;
		}
		if (tok->type == T_WORD)
			cmd->argv[i++] = ft_strdup(tok->value);
		tok = tok->next;
	}
	cmd->argv[i] = NULL;
	/* 5) Avancer le flux */
	*tokens = scan;
	/* 6) Rien du tout ? -> NULL */
	if (i == 0 && cmd->redirs == NULL)
		return (free_ast(cmd), NULL);
	return (cmd);
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
