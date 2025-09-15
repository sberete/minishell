/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_data.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:47:47 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:51:54 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_data_tmp(t_data *data)
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
	{
		free_ast(data->ast);
		data->ast = NULL;
	}
}

void	free_data(t_data *data)
{
	if (!data)
		return ;
	free_data_tmp(data);
	free_env(&data->env);
}
