/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/08/22 00:12:51 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *read_input(t_data *data)
{
	char *line;

	line = readline("minishell > ");
	if (!line)
	{
		printf("exit\n");
		rl_clear_history();
		free_data(data);
		exit(0);
	}
	if (*line)
		add_history(line);
	return line;
}

int	main(void)
{
	t_data data;

	memset(&data, 0, sizeof(t_data));
	while (1)
	{
		data.line = read_input(&data);
		if (tokenize_line(&data) == 0)
		{
			data.ast = parse_sequence(&data.tokens);  // <- Appel du parseur
			if (data.ast)
				print_ast(data.ast, 0); // <- Debug, afficher l’arbre
			else
				printf("Erreur: parsing\n");
		}
		else
		{
			print_syntax_error("tokenization");
			free_data(&data);
			continue ;
		}
		// print_tokens(data.tokens);
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