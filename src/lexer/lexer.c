/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:23 by sberete           #+#    #+#             */
/*   Updated: 2025/09/01 19:44:30 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_token(t_data *data, int *i)
{
	char	*token;
	char	*part;

	token = NULL;
	part = NULL;
	skip_spaces(data, i);
	if (!data->line[*i])
		return (NULL);
	part = extract_operator(data, i);
	if (part)
		return (part);
	while (data->line[*i] && data->line[*i] != ' '
		&& !is_operator_start(data->line[*i]))
	{
		if (data->line[*i] == '\'' || data->line[*i] == '"')
			part = extract_quoted(data, i);
		else
			part = extract_simple(data, i);
		if (!part)
			return (free(token), NULL);
		token = append_token_part(token, part);
		if (!token) // échec join/dup
			return (NULL);
	}
	return (token);
}

int	tokenize_line(t_data *data)
{
	int		i;
	char	*value;
	t_token	*new;
	t_token	*tail;

	i = 0;
	t_token *head = NULL; // <<< liste locale
	tail = NULL;
	data->tokens = NULL; // <<< on assigne à la fin seulement si succès
	while (data->line[i])
	{
		value = extract_token(data, &i);
		// fin de ligne après espaces
		if (!value)
		{
			// si on est vraiment à la fin -> OK (pas une erreur)
			if (!data->line[i])
				break ;
			// sinon, erreur d'extraction -> free la liste partielle
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
			// new_token_node a déjà free(value) en cas d'échec
			free_token_list(&head);
			return (1);
		}
		// chainage local
		if (!head)
			head = new;
		else
			tail->next = new;
		tail = new;
	}
	// succès: on publie la liste
	data->tokens = head;
	return (0);
}
