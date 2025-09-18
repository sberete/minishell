/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:23 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:55:56 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	token_push(t_token **list, char *value)
{
	t_token	*node;

	node = new_token_node(value);
	if (!node)
		return (1);
	if (!add_token_back(list, node))
	{
		free(node->value);
		free(node);
		return (1);
	}
	return (0);
}

static int	handle_token_value(t_token **list, char *value)
{
	if (!value)
		return (1);
	if (*value == '\0')
	{
		free(value);
		return (0);
	}
	return (token_push(list, value));
}

static int	has_more_nonblank(t_data *d, int i)
{
	int	j;

	j = i;
	skip_spaces(d, &j);
	if (!d->line[j])
		return (0);
	return (1);
}

int	tokenize_line(t_data *data)
{
	int		i;
	char	*value;

	if (!data || !data->line)
		return (1);
	data->tokens = NULL;
	i = 0;
	while (data->line[i])
	{
		if (!has_more_nonblank(data, i))
			break ;
		value = extract_token(data, &i);
		if (!value)
		{
			free_token_list(&data->tokens);
			return (1);
		}
		if (handle_token_value(&data->tokens, value) != 0)
		{
			free_token_list(&data->tokens);
			return (1);
		}
	}
	return (0);
}
