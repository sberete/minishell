/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:28:37 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 14:24:53 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	read_input(t_data *data)
{
	char	*line;
	int		i;

	line = readline("minishell > ");
	data->line = line;
	if (line == NULL)
	{
		printf("exit\n");
		rl_clear_history();
		return (0);
	}
	i = 0;
	skip_spaces(data, &i);
	if (line[i] == '\0')
	{
		free(line);
		data->line = NULL;
		return (2);
	}
	add_history(line);
	return (1);
}
