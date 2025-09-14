/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:23 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 17:59:09 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	tokenize_line(t_data *data)
{
	int		i;
	char	*value;
	t_token	*node;

	if (!data || !data->line)
		return (1);
	data->tokens = NULL;
	i = 0;
	while (data->line[i])
	{
		value = extract_token(data, &i);
		if (!value)
		{
			if (!data->line[i])
				break ;
			free_token_list(&data->tokens);
			return (1);
		}
		if (*value == '\0')
		{
			free(value);
			continue ;
		}
		node = new_token_node(value);
		if (!node || !add_token_back(&data->tokens, node))
		{
			if (node)
			{
				free(node->value);
				free(node);
			}
			else
				free(value);
			free_token_list(&data->tokens);
			return (1);
		}
	}
	return (0);
}
