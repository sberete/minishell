/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:13 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 20:00:48 by sxrimu           ###   ########.fr       */
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

/* Ajoute un argument (dup de s) et son flag can_expand à un NODE_CMD */
bool	add_arg(t_ast *cmd, const char *s, bool can_expand)
{
	size_t	len;
	char	**nargv;
	bool	*nflags;
	size_t	i;

	if (!cmd || !s)
		return (false);
	len = ft_tablen(cmd->argv);
	nargv = (char **)malloc(sizeof(char *) * (len + 2));
	if (!nargv)
		return (false);
	nflags = (bool *)malloc(sizeof(bool) * (len + 1));
	if (!nflags)
	{
		free(nargv);
		return (false);
	}
	i = 0;
	while (i < len)
	{
		nargv[i] = cmd->argv[i];
		nflags[i] = cmd->argv_can_expand[i];
		i++;
	}
	nargv[len] = ft_strdup(s);
	if (!nargv[len])
	{
		free(nflags);
		free(nargv);
		return (false);
	}
	nflags[len] = can_expand;
	nargv[len + 1] = NULL;

	free(cmd->argv);
	free(cmd->argv_can_expand);
	cmd->argv = nargv;
	cmd->argv_can_expand = nflags;
	return (true);
}

/* ------------------------- Redirections ---------------------------------- */

t_redir	*new_redir(t_redir_type type, char *filename, char *delim,
					bool filename_can_expand, bool delim_can_expand)
{
	t_redir	*r;

	r = (t_redir *)malloc(sizeof(t_redir));
	if (!r)
	{
		free(filename);
		free(delim);
		return (NULL);
	}
	r->type = type;
	r->filename = filename;  /* ownership pris */
	r->delim = delim;        /* ownership pris */
	r->fd = -1;
	r->filename_can_expand = filename_can_expand;
	r->delim_can_expand = delim_can_expand;
	r->next = NULL;
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
