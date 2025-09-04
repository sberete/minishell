/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:23 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 00:48:00 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_token(t_data *data, int *i)
{
	int		in_s;
	int		in_d;
	int		start;
	char	*op;

	skip_spaces(data, i);
	if (!data->line[*i])
		return (NULL);

	/* opérateur ? */
	op = extract_operator(data, i);
	if (op)
		return (op);

	/* mot complet avec quotes conservées */
	in_s = 0;
	in_d = 0;
	start = *i;
	while (data->line[*i])
	{
		char c = data->line[*i];

		/* si on n'est pas dans des quotes, fin du mot sur blanc ou opérateur */
		if (!in_s && !in_d)
		{
			if (c == ' ' || is_operator_start(c))
				break ;
		}
		/* toggle quotes (et on avance) */
		if (c == '\'' && !in_d)
			in_s = !in_s;
		else if (c == '\"' && !in_s)
			in_d = !in_d;
		(*i)++;
	}
	if (in_s || in_d)
	{
		print_syntax_error("unclosed quote");
		return (NULL);
	}
	/* duplique tel quel, quotes incluses */
	return (ft_substr(data->line, start, *i - start));
}

int	tokenize_line(t_data *data)
{
	int		i;
	char	*value;
	t_token	*new;
	t_token	*tail;
	t_token	*head;

	i = 0;
	head = NULL;
	tail = NULL;
	data->tokens = NULL;
	while (data->line[i])
	{
		value = extract_token(data, &i);
		/* fin de ligne après espaces */
		if (!value)
		{
			if (!data->line[i])
				break ;
			free_token_list(&head);
			return (1);
		}
		if (*value == '\0')
		{
			free(value);
			continue ;
		}
		new = new_token_node(value);
		if (!new)
		{
			/* new_token_node a déjà free(value) en cas d'échec */
			free_token_list(&head);
			return (1);
		}
		if (!head)
			head = new;
		else
			tail->next = new;
		tail = new;
	}
	data->tokens = head;
	return (0);
}