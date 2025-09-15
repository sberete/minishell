/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 23:48:50 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 23:51:50 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_indent(int depth)
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

static char	*redir_name(t_redir_type t)
{
	if (t == REDIR_IN)
		return ("<");
	if (t == REDIR_OUT)
		return (">");
	if (t == REDIR_APPEND)
		return (">>");
	if (t == REDIR_HEREDOC)
		return ("<<");
	return ("?");
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

static void	print_redirs(t_redir *r, int depth)
{
	char	*fname;
	char	*delim;
	char	*fexp;
	char	*dexp;

	while (r)
	{
		fname = "(null)";
		delim = "(null)";
		fexp = "false";
		dexp = "false";
		if (r->filename)
			fname = r->filename;
		if (r->delim)
			delim = r->delim;
		if (r->filename_can_expand)
			fexp = "true";
		if (r->delim_can_expand)
			dexp = "true";
		print_indent(depth);
		printf("  REDIR: %-2s  filename=%s  delim=%s  fexp=%s  dexp=%s\n",
			redir_name(r->type), fname, delim, fexp, dexp);
		r = r->next;
	}
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
