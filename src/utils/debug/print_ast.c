/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 22:39:38 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:39:40 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_indent(int depth)
{
	int	i;

	i = 0;
	while (i < depth)
	{
		printf("  ");
		i++;
	}
}

static char	*node_name(t_node_type t)
{
	if (t == NODE_CMD)
		return ("CMD");
	if (t == NODE_PIPE)
		return ("PIPE");
	if (t == NODE_AND)
		return ("AND");
	if (t == NODE_OR)
		return ("OR");
	if (t == NODE_SEQ)
		return ("SEQ");
	if (t == NODE_GROUP)
		return ("GROUP");
	return ("UNKNOWN_NODE");
}

static void	print_cmd_argv(char **argv, int depth)
{
	int		i;
	char	*val;

	print_indent(depth);
	printf("CMD:");
	if (argv)
	{
		i = 0;
		while (argv[i])
		{
			val = argv[i];
			if (val == NULL)
				val = "(null)";
			printf(" %s", val);
			i++;
		}
		if (i == 0)
			printf(" (no argv)");
	}
	else
		printf(" (no argv)");
	printf("\n");
}

void	print_ast(t_ast *n, int depth)
{
	if (!n)
	{
		print_indent(depth);
		printf("(nil)\n");
		return ;
	}
	print_indent(depth);
	printf("NODE: %s\n", node_name(n->type));
	if (n->type == NODE_CMD)
	{
		print_cmd_argv(n->argv, depth);
		print_redirs(n->redirs, depth);
	}
	if (n->left)
		print_ast(n->left, depth + 1);
	if (n->right)
		print_ast(n->right, depth + 1);
	if (n->child)
		print_ast(n->child, depth + 1);
}
