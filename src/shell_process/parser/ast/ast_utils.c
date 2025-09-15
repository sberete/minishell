/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:13 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 17:57:23 by sxrimu           ###   ########.fr       */
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

bool	add_arg(t_ast *cmd, char *s, bool can_expand)
{
	char	**nargv;
	bool	*nflags;
	size_t	len;
	size_t	i;

	if (!cmd || !s)
		return (false);
	len = ft_tablen(cmd->argv);
	nargv = malloc(sizeof(char *) * (len + 2));
	if (!nargv)
		return (false);
	nflags = malloc(sizeof(bool) * (len + 1));
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
