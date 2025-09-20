/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:28:37 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 18:03:59 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_blank(const char *s)
{
	size_t	i;

	if (!s)
		return (1);
	i = 0;
	while (s[i] && (s[i] == ' ' || s[i] == '\t'))
		i++;
	return (s[i] == '\0');
}

int	read_input(t_data *data)
{
	char	*line;

	line = readline("minishell > ");
	if (line == NULL)
	{
		printf("exit\n");
		rl_clear_history();
		return (0);
	}
	if (is_blank(line))
	{
		free(line);
		return (2);
	}
	add_history(line);
	data->line = line;
	if (g_exit_status == 128 + SIGINT)
		data->last_exit = 128 + SIGINT;
	return (1);
}
