/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 17:56:28 by sberete           #+#    #+#             */
/*   Updated: 2025/09/17 16:02:47 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	/* ⬇️ LIGNE CLÉ : si aucune commande après lexing -> noop, pas d’erreur */
	if (data->tokens == NULL)
		return (2);

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


