/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:49:42 by sberete           #+#    #+#             */
/*   Updated: 2025/09/06 16:48:19 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirs(t_redir *r)
{
	t_redir *next;

	while (r)
	{
		next = r->next;                 /* prendre next AVANT de free(r) */
		if (r->type == REDIR_HEREDOC)
			free(r->delim);             /* heredoc : on libère uniquement delim */
		else
			free(r->filename);          /* <, >, >> : on libère filename */
		free(r);
		r = next;
	}
}

void free_ast(t_ast *node)
{
    if (!node)
        return;
    if (node->argv) 
	{
        free_tab(node->argv);
        node->argv = NULL;              /* <- on remet bien à NULL */
    }
    if (node->redirs) 
	{
        free_redirs(node->redirs);
        node->redirs = NULL;            /* <- pareil pour la liste de redirs */
    }
    if (node->left)  
	{ 
		free_ast(node->left);  
		node->left = NULL; 
	}
    if (node->right) 
	{ 
		free_ast(node->right); 
		node->right = NULL; 
	}
    if (node->child) 
	{ 
		free_ast(node->child); 
		node->child = NULL; 
	}
    free(node);
}

