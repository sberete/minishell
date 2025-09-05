/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:13 by sberete           #+#    #+#             */
/*   Updated: 2025/09/01 19:39:00 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast	*new_ast_node(t_node_type type)
{
	t_ast	*node;

	node = ft_calloc(1, sizeof(t_ast));
	node->type = type;
	return (node);
}

t_redir	*new_redir(t_redir_type type, char *filename)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = filename;
	redir->next = NULL;
	return (redir);
}

void	add_redir(t_ast *cmd_node, t_redir *redir)
{
	t_redir	*tmp;

	if (!cmd_node->redirs)
		cmd_node->redirs = redir;
	else
	{
		tmp = cmd_node->redirs;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = redir;
	}
}
