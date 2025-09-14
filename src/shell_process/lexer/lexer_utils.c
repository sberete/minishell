/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:29 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 17:57:48 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* -------- helpers internes ----------------------------------------------- */

static bool	is_fully_single_quoted(const char *s)
{
	size_t	len;

	if (!s)
		return (false);
	len = ft_strlen(s);
	while (len && (s[len - 1] == ' ' || s[len - 1] == '\t'
			|| s[len - 1] == '\n' || s[len - 1] == '\r'))
		len--;
	return (len >= 2 && s[0] == '\'' && s[len - 1] == '\'');
}

/* -------- API utils lexer ------------------------------------------------ */

t_token	*new_token_node(char *value_own)
{
	t_token	*n;

	n = (t_token *)malloc(sizeof(t_token));
	if (!n)
	{
		free(value_own);
		return (NULL);
	}
	n->value = value_own; /* ownership: pas de strdup */
	n->type = get_token_type(n->value);
	n->can_expand = (n->type == T_WORD) && !is_fully_single_quoted(n->value);
	n->next = NULL;
	return (n);
}

bool	add_token_back(t_token **head, t_token *new)
{
	t_token	*cur;

	if (!head || !new)
		return (false);
	if (!*head)
	{
		*head = new;
		return (true);
	}
	cur = *head;
	while (cur->next)
		cur = cur->next;
	cur->next = new;
	return (true);
}

void	skip_spaces(t_data *data, int *i)
{
	if (!data || !data->line || !i)
		return ;
	while (data->line[*i] == ' ' || data->line[*i] == '\t'
		|| data->line[*i] == '\n' || data->line[*i] == '\r')
		(*i)++;
}

t_token_type	get_token_type(const char *str)
{
	if (!str)
		return (T_UNKNOWN);
	if (!ft_strcmp(str, "&&"))
		return (T_AND);
	else if (!ft_strcmp(str, "||"))
		return (T_OR);
	else if (!ft_strcmp(str, "|"))
		return (T_PIPE);
	else if (!ft_strcmp(str, "<<"))
		return (T_HEREDOC);
	else if (!ft_strcmp(str, ">>"))
		return (T_APPEND);
	else if (!ft_strcmp(str, "<"))
		return (T_REDIR_IN);
	else if (!ft_strcmp(str, ">"))
		return (T_REDIR_OUT);
	else if (!ft_strcmp(str, ";"))
		return (T_SEPARATOR);
	else if (!ft_strcmp(str, "("))
		return (T_PAREN_OPEN);
	else if (!ft_strcmp(str, ")"))
		return (T_PAREN_CLOSE);
	else if (!ft_strcmp(str, "*"))
		return (T_WILDCARD);
	else if (*str == '\0')
		return (T_END);
	return (T_WORD);
}
