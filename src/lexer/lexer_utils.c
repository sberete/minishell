#include "minishell.h"

t_token	*new_token_node(char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = value;
	token->type = get_token_type(value);
	token->next = NULL;
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
	if (!str) return (T_UNKNOWN);
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
	else if (*str == '\0') return (T_END);
	return (T_WORD);
}
