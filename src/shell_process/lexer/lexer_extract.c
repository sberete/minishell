/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extract.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:53:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 04:43:09 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_operator(t_data *data, int *i)
{
	int		len;
	char	*op;

	len = get_operator_len(&data->line[*i]);
	if (len <= 0)
		return (NULL);
	op = ft_substr(data->line, *i, len);
	if (!op)
		return (NULL);
	*i += len;
	return (op);
}

static void	print_unclosed_quote_error(int in_s, int in_d)
{
	char	q;

	q = '?';
	if (in_s)
		q = '\'';
	else if (in_d)
		q = '\"';
	ft_putstr_fd("minishell: syntax error: unexpected EOF ", 2);
	ft_putstr_fd("while looking for matching `", 2);
	ft_putchar_fd(q, 2);
	ft_putstr_fd("'\n", 2);
}

char	*extract_word(t_data *data, int *i)
{
	int	in_s;
	int	in_d;
	int	start;

	in_s = 0;
	in_d = 0;
	start = *i;
	while (data->line[*i])
	{
		if (!in_s && !in_d && (data->line[*i] == ' ' || data->line[*i] == '\t'
				|| data->line[*i] == '\n' || data->line[*i] == '\r'
				|| is_operator_start(data->line[*i])))
			break ;
		if (data->line[*i] == '\'' && !in_d)
			in_s = !in_s;
		else if (data->line[*i] == '\"' && !in_s)
			in_d = !in_d;
		(*i)++;
	}
	if (in_s || in_d)
	{
		print_unclosed_quote_error(in_s, in_d);
		return (NULL);
	}
	return (ft_substr(data->line, start, *i - start));
}

char	*extract_token(t_data *data, int *i)
{
	skip_spaces(data, i);
	if (!data->line[*i])
		return (NULL);
	if (get_operator_len(&data->line[*i]) > 0)
		return (extract_operator(data, i));
	return (extract_word(data, i));
}
