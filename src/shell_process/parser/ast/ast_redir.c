/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 22:21:15 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_redir_op(t_token_type t)
{
	if (t == T_REDIR_IN)
		return (1);
	if (t == T_REDIR_OUT)
		return (1);
	if (t == T_APPEND)
		return (1);
	if (t == T_HEREDOC)
		return (1);
	return (0);
}

int	parse_redirection(t_ast *cmd, t_token **tokp)
{
	t_token			*op;
	t_token			*arg;
	t_redir_type	rtype;
	char			*dup;
	t_redir			*r;

	if (!cmd || !tokp || !*tokp)
		return (1);
	op = *tokp;
	if (!is_redir_op(op->type))
		return (1);
	arg = op->next;
	if (!arg || arg->type != T_WORD)
		return (1);
	if (op->type == T_REDIR_IN)
		rtype = REDIR_IN;
	else if (op->type == T_REDIR_OUT)
		rtype = REDIR_OUT;
	else if (op->type == T_APPEND)
		rtype = REDIR_APPEND;
	else
		rtype = REDIR_HEREDOC;
	dup = ft_strdup(arg->value);
	if (!dup)
		return (1);
	r = new_redir(rtype, dup, arg->can_expand);
	if (!r)
	{
		free(dup);
		return (1);
	}
	add_redir(cmd, r);
	*tokp = arg->next;
	return (0);
}
