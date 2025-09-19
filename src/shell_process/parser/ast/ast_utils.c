/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:13 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 03:04:23 by sberete          ###   ########.fr       */
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

/* alloc des deux tableaux (argv len+2, flags len+1) */
static int	alloc_both(size_t len, char ***argvp, bool **flagsp)
{
	*argvp = (char **)malloc(sizeof(char *) * (len + 2));
	if (!*argvp)
		return (1);
	*flagsp = (bool *)malloc(sizeof(bool) * (len + 1));
	if (!*flagsp)
	{
		free(*argvp);
		*argvp = NULL;
		return (1);
	}
	return (0);
}

/* recopie les pointeurs existants + flags (ou false si tableau absent) */
static void	copy_old(const t_ast *cmd, char **na, bool *nf, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		na[i] = cmd->argv[i];
		if (cmd->argv_can_expand)
			nf[i] = cmd->argv_can_expand[i];
		else
			nf[i] = false;
		i++;
	}
	na[len] = NULL;
}

/* ajoute le nouvel élément et termine argv; pose aussi le flag */
static int	append_last(char ***pna, bool **pnf, const char *s, bool can_exp)
{
	char	**na;
	bool	*nf;
	size_t	i;
	char	*dup;

	if (!s)
		return (1);
	dup = ft_strdup(s);
	if (!dup)
		return (1);
	na = *pna;
	nf = *pnf;
	i = 0;
	while (na[i] != NULL)
		i++;
	na[i] = dup;
	na[i + 1] = NULL;
	nf[i] = can_exp;
	return (0);
}

/* API */
bool	add_arg(t_ast *cmd, char *s, bool can_expand)
{
	char	**na;
	bool	*nf;
	size_t	len;

	if (!cmd || !s)
		return (false);
	len = ft_tablen(cmd->argv);
	if (alloc_both(len, &na, &nf) != 0)
		return (false);
	copy_old(cmd, na, nf, len);
	if (append_last(&na, &nf, s, can_expand) != 0)
	{
		free(nf);
		free(na);
		return (false);
	}
	free(cmd->argv);
	free(cmd->argv_can_expand);
	cmd->argv = na;
	cmd->argv_can_expand = nf;
	return (true);
}
