/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_children_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 22:45:04 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:45:51 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	child_prepare_redirs(t_exec *ex, t_ast *cmd)
{
	(void)cmd;
	if (expand_redirs_inplace(ex->redirs, ex->data) != 0)
	{
		err_simple("redirection: expansion failed");
		free_ast(ex->data->ast);
		free_data(ex->data);
		exit(1);
	}
	if (apply_redirs(ex->redirs) != 0)
	{
		free_data(ex->data);
		exit(1);
	}
}

void	child_prepare_argv(t_ast *cmd, t_exec *ex)
{
	ex->argv = expand_argv_dup(cmd, ex->data);
	if (!ex->argv)
	{
		free_ast(ex->data->ast);
		free_env(&ex->data->env);
		exit(1);
	}
	if (!ex->argv[0])
	{
		free_ast(ex->data->ast);
		free_env(&ex->data->env);
		child_free_and_exit(ex->argv, NULL, NULL, 0);
	}
}
