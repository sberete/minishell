/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:15 by sberete           #+#    #+#             */
/*   Updated: 2025/08/25 21:55:29 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*parse_redirection(t_ast *cmd, t_token *tok)
{
	t_redir_type	rtype;

	if (tok->type == T_REDIR_IN)
		rtype = REDIR_IN;
	else if (tok->type == T_REDIR_OUT)
		rtype = REDIR_OUT;
	else if (tok->type == T_APPEND)
		rtype = REDIR_APPEND;
	else
		rtype = REDIR_HEREDOC;
	tok = tok->next;
	if (!tok || tok->type != T_WORD)
	{
		printf("Erreur: redirection sans fichier\n");
		free_ast(cmd);
		return (NULL);
	}
	add_redir(cmd, new_redir(rtype, ft_strdup(tok->value)));
	return (tok->next);
}
