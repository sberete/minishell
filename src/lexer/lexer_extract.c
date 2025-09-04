/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extract.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:53:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 00:50:47 by sxrimu           ###   ########.fr       */
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

char	*extract_word(t_data *data, int *i)
{
	int		in_s = 0, in_d = 0;
	int		start;
	char	*token;

	start = *i;
	while (data->line[*i])
	{
		char c = data->line[*i];
		if (!in_s && !in_d && (c == ' ' || is_operator_start(c)))
			break;
		if (c == '\'' && !in_d)
			in_s = !in_s;
		else if (c == '"' && !in_s)
			in_d = !in_d;
		(*i)++;
	}
	if (in_s || in_d)
	{
		print_syntax_error("unclosed quote");
		return (NULL);
	}
	token = ft_substr(data->line, start, *i - start); // inclut tout, quotes comprises
	return (token);
}



void	skip_spaces(t_data *data, int *i)
{
	while (data->line[*i] == ' ')
		(*i)++;
}
