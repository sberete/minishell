/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/08/15 21:03:01 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(void)
{
	t_data data;

	memset(&data, 0, sizeof(t_data));
	while (1)
	{
		data.line = readline("minishell > ");
		if (!data.line)
		{
			printf("exit\n");
			break ;
		}
		if (*data.line)
			add_history(data.line);
		if (tokenize_line(&data) == 0)
		{
			//créer l'ast
		}
		else
		{
			print_syntax_error("tokenization");
			free_data(&data);
			continue ;
		}
		print_tokens(data.tokens);
		free_data(&data);
	}
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/

/*
Tokenization
Parcer -> AST
*/