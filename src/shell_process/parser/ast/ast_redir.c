/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/07 20:59:33 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_redir_op(t_token_type t)
{
	return (t == T_REDIR_IN || t == T_REDIR_OUT || t == T_APPEND
		|| t == T_HEREDOC);
}

/* tok pointe sur l'opérateur (<, >, >>, <<) */
t_token	*parse_redirection(t_ast *cmd, t_token *tok)
{
	t_redir_type	rtype;
	t_token			*arg;
	t_redir			*r;
	char			*fname;
	char			*delim;

	if (!cmd || !tok || !is_redir_op(tok->type))
		return (tok);
	arg = tok->next;
	if (!arg || arg->type != T_WORD)
	{
		print_syntax_error("redirection");
		return (tok);
	}
	if (tok->type == T_REDIR_IN)
		rtype = REDIR_IN;
	else if (tok->type == T_REDIR_OUT)
		rtype = REDIR_OUT;
	else if (tok->type == T_APPEND)
		rtype = REDIR_APPEND;
	else
		rtype = REDIR_HEREDOC;
	if (rtype == REDIR_HEREDOC)
	{
		delim = ft_strdup(arg->value);
		if (!delim)
			return (tok);
		r = new_redir(REDIR_HEREDOC, NULL, delim);
		if (!r)
		{
			free(delim);
			return (tok);
		}
	}
	else
	{
		fname = ft_strdup(arg->value);
		if (!fname)
			return (tok);
		r = new_redir(rtype, fname, NULL);
		if (!r)
		{
			free(fname);
			return (tok);
		}
	}
	add_redir(cmd, r);
	return (arg->next);
}
