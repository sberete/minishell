/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:23 by sberete           #+#    #+#             */
/*   Updated: 2025/09/06 18:20:37 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_token(t_data *data, int *i)
{
	int		in_s;
	int		in_d;
	int		start;
	char	*op;
	char	c;

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
		c = data->line[*i];
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
	int		i = 0;
	char	*value;
	t_token	*tail = NULL;   // dernier maillon ajouté

	data->tokens = NULL;     // on construit directement dans data->tokens
	while (data->line[i])
	{
		value = extract_token(data, &i);
		if (!value)  // erreur ou fin de ligne
		{
			if (!data->line[i])
				break;
			free_token_list(&data->tokens);
			return (1);
		}
		if (*value == '\0')  // token vide -> ignorer
		{
			free(value);
			continue;
		}
		t_token *new = new_token_node(value);  // prend la propriété de value
		if (!new)
		{
			free_token_list(&data->tokens);
			return (1);
		}
		if (!data->tokens)      // premier élément
			data->tokens = new;
		else
			tail->next = new;
		tail = new;             // avance le tail
	}
	return (0);
}

