/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/09 18:36:01 by sxrimu           ###   ########.fr       */
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

static int is_redir_type(t_token_type t)
{
    return (t == T_REDIR_IN || t == T_REDIR_OUT
         || t == T_APPEND   || t == T_HEREDOC);
}

t_ast *parse_command(t_token **tokens)
{
    t_token *tok = *tokens;
    t_ast   *cmd = NULL;
    t_token *scan;
    int      argc = 0;
    int      i;
    t_token *next;

    /* 0) Redirs en tête */
    while (tok && is_redir_type(tok->type))
    {
        if (!cmd)
        {
            cmd = new_ast_node(NODE_CMD);
            if (!cmd) return NULL;
        }
        next = parse_redirection(cmd, tok);      /* consomme op + WORD */
        if (next == tok)                         /* erreur: pas d'avancement */
            return free_ast(cmd), NULL;
        tok = next;                              /* avance (NULL autorisé) */
    }

    /* CMD “vide” mais avec redirs ? OK */
    if (cmd && (!tok || tok->type != T_WORD))
    {
        *tokens = tok;
        return cmd;
    }

    /* 1) Compter les WORD (skip redirs intercalées) */
    scan = tok;
    while (scan && scan->type != T_PIPE && scan->type != T_AND
           && scan->type != T_OR && scan->type != T_SEPARATOR
           && scan->type != T_PAREN_CLOSE)
    {
        if (is_redir_type(scan->type))
        {
            if (!scan->next || scan->next->type != T_WORD)
                return free_ast(cmd), NULL;
            scan = scan->next->next;             /* skip op + WORD */
            continue;
        }
        if (scan->type == T_WORD)
            argc++;
        scan = scan->next;
    }

    /* 2) S'assurer d'avoir un nœud commande */
    if (!cmd)
    {
        cmd = new_ast_node(NODE_CMD);
        if (!cmd) return NULL;
    }

    /* 3) Allouer argv + flags */
    cmd->argv = (char **)malloc(sizeof(char *) * (argc + 1));
    if (!cmd->argv) return free_ast(cmd), NULL;

    cmd->argv_can_expand = (bool *)malloc(sizeof(bool) * argc);
    if (!cmd->argv_can_expand) { free(cmd->argv); return free_ast(cmd), NULL; }

    /* 4) Remplir argv & redirs entre [tok..scan) */
    i = 0;
    while (tok && tok != scan)
    {
        if (is_redir_type(tok->type))
        {
            next = parse_redirection(cmd, tok);
            if (next == tok)                     /* erreur */
                return free_ast(cmd), NULL;
            tok = next;
            continue;
        }
        if (tok->type == T_WORD)
        {
            cmd->argv[i] = ft_strdup(tok->value);
            if (!cmd->argv[i]) return free_ast(cmd), NULL;
            cmd->argv_can_expand[i] = tok->can_expand;  /* copie du flag */
            i++;
        }
        tok = tok->next;
    }
    cmd->argv[i] = NULL;

    /* 5) Avancer le flux */
    *tokens = scan;

    /* 6) Rien du tout ? -> NULL */
    if (i == 0 && cmd->redirs == NULL)
        return free_ast(cmd), NULL;

    return cmd;
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
