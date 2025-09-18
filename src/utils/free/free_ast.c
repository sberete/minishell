/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:49:42 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 00:50:42 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirs(t_redir *r)
{
	t_redir	*n;

	while (r)
	{
		n = r->next;
		free(r->filename);
		if (r->fd >= 0)
			close(r->fd);
		free(r);
		r = n;
	}
}

void	free_ast(t_ast *n)
{
	size_t	i;

	if (!n)
		return ;
	free_ast(n->left);
	free_ast(n->right);
	free_ast(n->child);
	if (n->type == NODE_CMD)
	{
		if (n->argv)
		{
			i = 0;
			while (n->argv[i])
			{
				free(n->argv[i]);
				i++;
			}
			free(n->argv);
		}
		free_redirs(n->redirs);
	}
	free(n);
}
