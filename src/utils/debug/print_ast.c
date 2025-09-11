/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:50:30 by sberete           #+#    #+#             */
/*   Updated: 2025/09/09 18:45:37 by sxrimu           ###   ########.fr       */
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

static void	print_redirs(t_redir *redir, int depth)
{
	while (redir)
	{
		const char *label = "";
		const char *name = "(null)";
		const char *suffix = "";

		print_indent(depth);
		printf("REDIR: ");

		if (redir->type == REDIR_IN)
			label = "< ";
		else if (redir->type == REDIR_OUT)
			label = "> ";
		else if (redir->type == REDIR_APPEND)
			label = ">> ";
		else if (redir->type == REDIR_HEREDOC)
			label = "<< ";

		if (redir->type == REDIR_HEREDOC)
		{
			if (redir->delim)
				name = redir->delim;
			if (!redir->delim_can_expand)
				suffix = " (noexp)";
		}
		else
		{
			if (redir->filename)
				name = redir->filename;
			if (!redir->filename_can_expand)
				suffix = " (noexp)";
		}
		printf("%s%s%s\n", label, name, suffix);

		redir = redir->next;
	}
}

static void	print_node_type(t_ast *node)
{
	if (node->type == NODE_PIPE)
		printf("PIPE\n");
	else if (node->type == NODE_AND)
		printf("AND (&&)\n");
	else if (node->type == NODE_OR)
		printf("OR (||)\n");
	else if (node->type == NODE_SEQ)
		printf("SEQ (;)\n");
	else if (node->type == NODE_GROUP)
		printf("GROUP (subshell)\n");
}

void	print_ast(t_ast *node, int depth)
{
	int	i;

	if (!node)
		return;
	print_indent(depth);
	if (node->type == NODE_CMD)
	{
		printf("CMD: ");
		i = 0;
		while (node->argv && node->argv[i])
		{
			const char *word = node->argv[i];
			const char *suffix = "";
			if (node->argv_can_expand && !node->argv_can_expand[i])
				suffix = "(noexp)";
			printf("%s%s ", word, suffix);
			i++;
		}
		printf("\n");
		print_redirs(node->redirs, depth + 1);
	}
	else
		print_node_type(node);
	print_ast(node->left, depth + 1);
	print_ast(node->right, depth + 1);
	print_ast(node->child, depth + 1);
}
