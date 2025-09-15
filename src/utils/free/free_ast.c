/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:49:42 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 22:22:12 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirs(t_redir *r)
{
	t_redir	*next;

	while (r)
	{
		next = r->next;
		if (r->fd >= 0)
			close(r->fd);
		if (r->filename)
			free(r->filename);
		if (r->delim)
			free(r->delim);
		free(r);
		r = next;
	}
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	free_ast(node->child);
	if (node->argv)
		free_tab(node->argv);
	if (node->argv_can_expand)
		free(node->argv_can_expand);
	if (node->redirs)
		free_redirs(node->redirs);
	free(node);
}
