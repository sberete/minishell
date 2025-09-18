/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:25:05 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:25:06 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redir	*new_redir(t_redir_type type, char *text, bool can_exp)
{
	t_redir	*r;

	r = (t_redir *)malloc(sizeof(t_redir));
	if (!r)
		return (NULL);
	r->type = type;
	r->fd = -1;
	r->next = NULL;
	r->filename = NULL;
	r->delim = NULL;
	r->filename_can_expand = false;
	r->delim_can_expand = false;
	if (type == REDIR_HEREDOC)
	{
		r->delim = text;
		r->delim_can_expand = can_exp;
	}
	else
	{
		r->filename = text;
		r->filename_can_expand = can_exp;
	}
	return (r);
}

void	add_redir(t_ast *cmd_node, t_redir *redir)
{
	t_redir	*cur;

	if (!cmd_node || !redir)
		return ;
	if (!cmd_node->redirs)
	{
		cmd_node->redirs = redir;
		return ;
	}
	cur = cmd_node->redirs;
	while (cur->next)
		cur = cur->next;
	cur->next = redir;
}
