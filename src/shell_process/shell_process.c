/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 17:56:28 by sberete           #+#    #+#             */
/*   Updated: 2025/09/07 17:59:52 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	shell_process(t_data *data)
{
	t_token	*head;
	t_token	*cur;

	if (tokenize_line(data) != 0)
	{
		print_syntax_error("tokenization");
		if (data->tokens)
			free_token_list(&data->tokens);
		return (1);
	}
	print_tokens(data->tokens);
	head = data->tokens;
	cur = head;
	data->ast = parse_sequence(&cur);
	free_token_list(&head);
	data->tokens = NULL;
	if (!data->ast)
	{
		print_syntax_error("parser");
		return (1);
	}
	print_ast(data->ast, 0);
	/* même si l'exec/expand est commenté, il FAUT libérer l'AST */
	// if (expand_ast(data->ast, data) == 0)
	// 	exec_ast(data);
	free_ast(data->ast);
	data->ast = NULL;
	return (0);
}
