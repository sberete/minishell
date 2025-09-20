/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:03:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:46:49 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	exec_prepare_heredocs_once(t_ast *node, t_data *data, int depth)
{
	if (depth != 1)
		return (0);
	if (prepare_all_heredocs(node, data) != 0)
	{
		err_simple("heredoc: aborted");
		return (130);
	}
	return (0);
}

static int	exec_dispatch_node(t_ast *node, t_data *data)
{
	if (node->type == NODE_CMD)
		return (exec_cmd_node(node, data));
	else if (node->type == NODE_PIPE)
		return (exec_pipeline_node(node, data));
	else if (node->type == NODE_AND || node->type == NODE_OR)
		return (exec_and_or_node(node, data));
	else if (node->type == NODE_SEQ)
		return (exec_seq_node(node, data));
	else if (node->type == NODE_GROUP)
		return (exec_group_node(node, data));
	return (1);
}

int	exec_ast(t_ast *node, t_data *data)
{
	static int	depth;
	int			st;

	if (!node)
		return (0);
	depth++;
	st = exec_prepare_heredocs_once(node, data, depth);
	if (st != 0)
	{
		depth--;
		return (st);
	}
	st = exec_dispatch_node(node, data);
	depth--;
	data->last_exit = st;
	return (st);
}
