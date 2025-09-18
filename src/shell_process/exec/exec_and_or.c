// #include "minishell.h"

// int	exec_and_or_node(t_ast *n, t_data *d)
// {
// 	int	st;

// 	st = exec_ast(n->left, d);
// 	if (n->type == NODE_AND)
// 	{
// 		if (st == 0)
// 			st = exec_ast(n->right, d);
// 	}
// 	else
// 	{
// 		if (st != 0)
// 			st = exec_ast(n->right, d);
// 	}
// 	d->last_exit = st;
// 	return (st);
// }