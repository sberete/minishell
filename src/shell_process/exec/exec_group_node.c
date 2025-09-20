/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_group_node.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:06:18 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 18:12:30 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_subshell(t_ast *child, t_data *data)
{
	pid_t	pid;
	int		st;

	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		signals_setup_child();
		st = exec_ast(child, data);
		// free_ast(child);
		// free_ast(*data->ast_root);
		// free_ast(*data->(ast_rootd));
		free_env(&data->env);
		free_data(data);
		exit(127);
	}
	signals_setup_parent_execwait();
	return (wait_and_get_status(pid));
}

int	exec_group_node(t_ast *n, t_data *data)
{
	int	st;

	if (!n)
		return (1);
	data->ast_root = &n;
	st = run_subshell(n->child, data);
	data->last_exit = st;
	return (st);
}
