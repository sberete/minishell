/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extract.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:53:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/03 22:45:21 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_operator(t_data *data, int *i)
{
	int		op_len;
	char	*token;

	op_len = get_operator_len(&data->line[*i]);
	if (op_len > 0)
	{
		token = ft_substr(data->line, *i, op_len);
		if (!token)
			return (NULL);
		*i += op_len;
		return (token);
	}
	return (NULL);
}

char	*extract_simple(t_data *data, int *i)
{
	int	start;

	start = *i;
	while (data->line[*i] && data->line[*i] != ' '
		&& !is_operator_start(data->line[*i]) && data->line[*i] != '\''
		&& data->line[*i] != '"')
		(*i)++;
	return (ft_substr(data->line, start, *i - start));
}

char	*extract_quoted(t_data *data, int *i)
{
	char	quote;
	int		start;
	char	*token;

	quote = data->line[*i];
	start = ++(*i); // skip quote opening
	while (data->line[*i] && data->line[*i] != quote)
		(*i)++;
	if (!data->line[*i])
	{
		print_syntax_error("unclosed quote");
		return (NULL);
	}
	token = ft_substr(data->line, start, *i - start);
	if (!token)
		return (NULL);
	(*i)++; // skip quote closing
	return (token);
}

void	skip_spaces(t_data *data, int *i)
{
	while (data->line[*i] == ' ')
		(*i)++;
}

char	*append_token_part(char *token, char *part)
{
	char *joined;

	if (!part)
		return (token);
	if (token)
	{
		joined = ft_strjoin(token, part);
		if (!joined)
		{
			free(token);
			free(part);
			return (NULL); // <<< propage l’erreur au dessus
		}
		free(token);
		free(part);
		return (joined);
	}
	// pas encore de token: dupliquer part
	joined = ft_strdup(part);
	free(part);
	return (joined); // peut être NULL si strdup échoue
}
