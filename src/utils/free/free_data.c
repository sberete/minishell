/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_data.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:47:47 by sberete           #+#    #+#             */
/*   Updated: 2025/09/04 17:07:10 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_data(t_data *data)
{
	if (!data)
		return ;
	if (data->line)
	{
		free(data->line);
		data->line = NULL;
	}
	if (data->tokens)
		free_token_list(&data->tokens);
	if (data->ast)
		free_ast(data->ast);
	env_clear(&data->env);
}
