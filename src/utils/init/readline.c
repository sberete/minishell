/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:28:37 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 01:51:38 by sberete          ###   ########.fr       */
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
		if (g_exit_status)
		{
			g_exit_status = 0;
			data->last_exit = 130;
			return (2);
		}
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
	return (1);
}
