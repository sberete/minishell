/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:49:42 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 02:41:32 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirs(t_redir *r)
{
	t_redir	*n;

	while (r)
	{
		n = r->next;
		if (r->filename)
			free(r->filename);
		if (r->delim)
			free(r->delim);
		free(r);
		r = n;
	}
}

static void	free_cmd_fields(t_ast *n)
{
	size_t	i;

	i = 0;
	if (n->argv)
	{
		while (n->argv[i])
		{
			free(n->argv[i]);
			i++;
		}
		free(n->argv);
	}
	if (n->argv_can_expand)
		free(n->argv_can_expand);
	if (n->redirs)
		free_redirs(n->redirs);
}

void	free_ast(t_ast *n)
{
	if (!n)
		return ;
	if (n->left)
		free_ast(n->left);
	if (n->right)
		free_ast(n->right);
	if (n->child)
		free_ast(n->child);
	if (n->type == NODE_CMD)
		free_cmd_fields(n);
	free(n);
}
