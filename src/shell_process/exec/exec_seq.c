#include "minishell.h"

int	exec_seq_node(t_ast *n, t_data *data)
{
	if (!n)
		return (1);
	(void)exec_ast(n->left, data);
	return (exec_ast(n->right, data));
}