/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:29 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 00:46:10 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*new_token_node(char *value)
{
	t_token	*token;
	size_t len;
	
	token = malloc(sizeof(t_token));
	if (!token)
	{
		free(value);
		return (NULL);
	}
	token->value = value;
	token->type = get_token_type(value);
	token->next = NULL;
	token->to_expand = TOEXP_AUTO;
	if (token->type == T_WORD && token->value)
	{
		len = ft_strlen(token->value);
		if (len >= 2 && token->value[0] == '\'' && token->value[len - 1] == '\'')
			token->to_expand = TOEXP_SQUOTE; // tout le mot est '...'
		else if (len >= 2 && token->value[0] == '\"' && token->value[len - 1] == '\"')
			token->to_expand = TOEXP_DQUOTE; // tout le mot est "..."
		// sinon, laisse à AUTO
	}
	return (token);
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
