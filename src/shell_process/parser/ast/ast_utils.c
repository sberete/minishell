/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:13 by sberete           #+#    #+#             */
/*   Updated: 2025/09/09 18:31:34 by sxrimu           ###   ########.fr       */
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

t_redir *new_redir(t_redir_type type,
                   char *filename, char *delim,
                   bool filename_can_expand, bool delim_can_expand)
{
    t_redir *r = (t_redir *)malloc(sizeof(*r));
    if (!r)
        return NULL;
    r->type = type;
    r->filename = filename;                 /* NULL si heredoc */
    r->delim = delim;                       /* NULL si pas heredoc */
    r->fd = -1;                             /* important pour free/close */
    r->filename_can_expand = filename_can_expand;
    r->delim_can_expand = delim_can_expand;
    r->next = NULL;
    return r;
}

void add_redir(t_ast *cmd_node, t_redir *redir)
{
    t_redir *cur;

    if (!cmd_node || !redir)
        return;
    if (!cmd_node->redirs)
    {
        cmd_node->redirs = redir;
        return;
    }
    cur = cmd_node->redirs;
    while (cur->next)
        cur = cur->next;
    cur->next = redir;
}
