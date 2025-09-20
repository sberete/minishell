/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 17:56:28 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:18:59 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_AST_test(t_ast *command);

int	shell_process(t_data *data)
{
	t_token	*head;

	if (tokenize_line(data) != 0)
	{
		print_syntax_error("tokenization");
		if (data->tokens)
			free_token_list(&data->tokens);
		set_exit_status(2);
		return (1);
	}
	head = data->tokens;
	data->ast = parse_entry(data);
	free_token_list(&head);
	data->tokens = NULL;
	if (!data->ast)
	{
		print_syntax_error("parser");
		set_exit_status(2);
		return (1);
	}
	exec_ast(data->ast, data);
	free_ast(data->ast);
	data->ast = NULL;
	return (0);
}
