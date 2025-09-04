/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 00:48:20 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*read_input(t_data *data)
{
	char	*line;

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
	return (line);
}

int	main(int argc, char **argv, char **env)
{
	t_data	data;

	data = data_init(argc, argv, env);
	while (1)
	{
		data.line = read_input(&data);
		if (tokenize_line(&data) == 0)
		{
			print_tokens(data.tokens);
			// data.ast = parse_sequence(&data.tokens); // <- Appel du parseur
			// if (data.ast)
			// {
			// 	print_ast(data.ast, 1);
			// 	exec_ast(data.ast, &data);
			// }
			// else
			// 	printf("Erreur: parsing\n");
		}
		else
		{
			print_syntax_error("tokenization");
			free_data(&data);
			continue ;
		}
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