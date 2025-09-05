/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/01 19:49:11 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*parse_redirection(t_ast *cmd, t_token *op)
{
	t_redir_type	rtype;
	t_token			*tok;
	t_redir			*r;

	if (op->type == T_REDIR_IN)
		rtype = REDIR_IN;
	else if (op->type == T_REDIR_OUT)
		rtype = REDIR_OUT;
	else if (op->type == T_APPEND)
		rtype = REDIR_APPEND;
	else
		rtype = REDIR_HEREDOC;
	tok = op->next;
	if (!tok || tok->type != T_WORD)
	{
		printf("Erreur: redirection sans fichier\n");
		return (op);
	}
	r = new_redir(rtype, ft_strdup(tok->value));
	if (!r || !r->filename)
	{
		if (r)
			free(r);
		return (op);
	}
	add_redir(cmd, r);
	return (tok->next);
}
