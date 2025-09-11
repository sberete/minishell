/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:29 by sberete           #+#    #+#             */
/*   Updated: 2025/09/09 19:36:13 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* helper: longueur sans espaces/tabs/CRLF finaux */
static size_t rtrim_len(const char *s)
{
    size_t len;

    if (!s) return 0;
    len = ft_strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t'
                    || s[len - 1] == '\r' || s[len - 1] == '\n'))
        len--;
    return len;
}

static inline bool is_fully_single_quoted_trim(const char *s)
{
    size_t len = rtrim_len(s);
    if (len < 2) return false;
    return (s[0] == '\'' && s[len - 1] == '\'');
}

t_token *new_token_node(char *value)
{
    t_token *token = malloc(sizeof(*token));
    if (!token) { free(value); return NULL; }

    token->value      = value;
    token->type       = get_token_type(value);
    token->next       = NULL;
    token->can_expand = true;

    if (token->type == T_WORD && token->value)
        token->can_expand = !is_fully_single_quoted_trim(token->value);

    return token;
}

bool	add_token_back(t_token **head, t_token *new)
{
	t_token	*tmp;

	if (!new)
		return (false);
	if (!*head)
	{
		*head = new;
		return (true);
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
	return (true);
}

void	skip_spaces(t_data *data, int *i)
{
	while (data->line[*i] == ' ' || data->line[*i] == '\t')
		(*i)++;
}


t_token_type	get_token_type(const char *str)
{
	if (!str)
		return (T_UNKNOWN);
	if (ft_strcmp(str, "&&") == 0)
		return (T_AND);
	else if (ft_strcmp(str, "||") == 0)
		return (T_OR);
	else if (ft_strcmp(str, "|") == 0)
		return (T_PIPE);
	else if (ft_strcmp(str, "<<") == 0)
		return (T_HEREDOC);
	else if (ft_strcmp(str, ">>") == 0)
		return (T_APPEND);
	else if (ft_strcmp(str, "<") == 0)
		return (T_REDIR_IN);
	else if (ft_strcmp(str, ">") == 0)
		return (T_REDIR_OUT);
	else if (ft_strcmp(str, ";") == 0)
		return (T_SEPARATOR);
	else if (ft_strcmp(str, "(") == 0)
		return (T_PAREN_OPEN);
	else if (ft_strcmp(str, ")") == 0)
		return (T_PAREN_CLOSE);
	else if (ft_strcmp(str, "*") == 0)
		return (T_WILDCARD);
	else if (*str == '\0')
		return (T_END);
	return (T_WORD);
}
