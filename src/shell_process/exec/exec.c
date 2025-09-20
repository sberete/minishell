/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:03:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:06:36 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	exec_prepare_heredocs_once(t_ast *node, t_data *data, int depth)
{
	if (depth != 0)
		return (0);
	if (prepare_all_heredocs(node, data) != 0)
	{
		err_simple("heredoc: aborted");
		return (130);
	}
	return (0);
}

int	exec_ast(t_ast *node, t_data *data)
{
	static int	depth;
	int			st;

	if (!node)
		return (0);
	depth = 0;
	st = exec_prepare_heredocs_once(node, data, depth);
	if (st != 0)
		return (st);
	if (node->type == NODE_CMD)
		st = exec_cmd_node(node, data);
	else if (node->type == NODE_PIPE)
		st = exec_pipeline_node(node, data);
	else if (node->type == NODE_AND || node->type == NODE_OR)
		st = exec_and_or_node(node, data);
	else if (node->type == NODE_SEQ)
		st = exec_seq_node(node, data);
	else if (node->type == NODE_GROUP)
		st = exec_group_node(node, data);
	else
		st = 1;
	depth--;
	data->last_exit = st;
	return (st);
}
