/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:50:45 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 20:22:52 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int  is_redir(t_token_type t)
{
    return (t == T_REDIR_IN || t == T_REDIR_OUT
         || t == T_APPEND   || t == T_HEREDOC);
}

static int  is_binop(t_token_type t) { return (t == T_AND || t == T_OR || t == T_PIPE); }
static int  can_start_cmd(t_token_type t) { return (t == T_WORD || t == T_PAREN_OPEN || is_redir(t)); }

int parser_syntax(const t_token *t)
{
    int paren = 0;
    t_token_type prev = T_SEPARATOR; /* comme si on démarrait après ';' */

    while (t) {
        t_token_type tt = t->type;

        if (tt == T_UNKNOWN) return (print_syntax_error("unknown token"), 0);
        if (tt == T_END) break;

        if (tt == T_SEPARATOR) {
            if (prev == T_SEPARATOR || prev == T_PAREN_OPEN || is_binop(prev) || is_redir(prev))
                return (print_syntax_error("unexpected ';'"), 0);
            prev = tt; t = t->next; continue;
        }
        if (tt == T_PAREN_OPEN) {
            paren++; prev = tt; t = t->next;
            if (!t || t->type == T_PAREN_CLOSE || t->type == T_SEPARATOR || is_binop(t->type))
                return (print_syntax_error("empty group"), 0);
            continue;
        }
        if (tt == T_PAREN_CLOSE) {
            if (paren == 0) return (print_syntax_error("unexpected ')'"), 0);
            if (prev == T_SEPARATOR || prev == T_PAREN_OPEN || is_binop(prev) || is_redir(prev))
                return (print_syntax_error("unexpected ')'"), 0);
            paren--; prev = tt; t = t->next; continue;
        }
        if (is_binop(tt)) {
            if (prev == T_SEPARATOR || prev == T_PAREN_OPEN || is_binop(prev) || is_redir(prev))
                return (print_syntax_error("unexpected binary operator"), 0);
            if (!t->next || !can_start_cmd(t->next->type))
                return (print_syntax_error("operator at end"), 0);
            prev = tt; t = t->next; continue;
        }
        if (is_redir(tt)) {
            if (!t->next || t->next->type != T_WORD)
                return (print_syntax_error("redirection without target"), 0);
            prev = T_WORD; t = t->next->next; continue; /* consomme cible */
        }
        if (tt == T_WORD) { prev = tt; t = t->next; continue; }

        return (print_syntax_error("unexpected token"), 0);
    }
    if (paren != 0) return (print_syntax_error("unclosed '('"), 0);
    if (prev == T_SEPARATOR || prev == T_PAREN_OPEN || is_binop(prev) || is_redir(prev))
        return (print_syntax_error("unexpected end of line"), 0);
    return (1);
}

t_ast  *parse_entry(t_data *data)
{
    t_token *cur;
    t_ast   *root;

    if (!data || !data->tokens)
        return (NULL);
    if (!parser_syntax(data->tokens))
        return (NULL);
    cur = data->tokens;
    root = parse_sequence(&cur);
    if (!root)
        return (NULL);
    if (cur != NULL) /* il reste des tokens non consommés -> erreur */
    {
        free_ast(root);
        print_syntax_error("trailing tokens");
        return (NULL);
    }
    return (root);
}
