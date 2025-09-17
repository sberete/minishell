#include "minishell.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int	is_parent_builtin(char *name)
{
	return (name && (str_eq(name, "cd") || str_eq(name, "export")
			|| str_eq(name, "unset") || str_eq(name, "exit")));
}

/* PUBLIC: wrapper top-level (prépare heredocs une seule fois) */
int	exec_ast(t_ast *node, t_data *data)
{
	static int	depth;
	int			st;
	int			rc;

	if (!node)
		return (0);
	if (depth == 0)
	{
		rc = prepare_all_heredocs(node, data);
		if (rc != 0)
		{
			data->last_exit = rc;
			return (rc);
		}
	}
	depth++;
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

