/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:52:15 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:15:41 by sberete          ###   ########.fr       */
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

static int	redir_type_from_tok(t_token_type tt, t_redir_type *out)
{
	if (!out)
		return (1);
	if (tt == T_REDIR_IN)
	{
		*out = REDIR_IN;
		return (0);
	}
	if (tt == T_REDIR_OUT)
	{
		*out = REDIR_OUT;
		return (0);
	}
	if (tt == T_APPEND)
	{
		*out = REDIR_APPEND;
		return (0);
	}
	if (tt == T_HEREDOC)
	{
		*out = REDIR_HEREDOC;
		return (0);
	}
	return (1);
}

static int	validate_pair(t_token **tokp, t_token **op, t_token **arg)
{
	t_token	*t;

	if (!tokp || !*tokp || !op || !arg)
		return (1);
	t = *tokp;
	if (!is_redir_op(t->type))
		return (1);
	if (!t->next || t->next->type != T_WORD)
		return (1);
	*op = t;
	*arg = t->next;
	return (0);
}

static int	build_redir_node(t_token *op, t_token *arg, t_redir **out)
{
	t_redir_type	rt;
	char			*dup;
	t_redir			*r;

	if (!op || !arg || !out)
		return (1);
	if (redir_type_from_tok(op->type, &rt) != 0)
		return (1);
	dup = ft_strdup(arg->value);
	if (!dup)
		return (1);
	r = new_redir(rt, dup, arg->can_expand);
	if (!r)
	{
		free(dup);
		return (1);
	}
	*out = r;
	return (0);
}

int	parse_redirection(t_ast *cmd, t_token **tokp)
{
	t_token	*op;
	t_token	*arg;
	t_redir	*r;

	if (!cmd)
		return (1);
	if (validate_pair(tokp, &op, &arg) != 0)
		return (1);
	if (build_redir_node(op, arg, &r) != 0)
		return (1);
	add_redir(cmd, r);
	*tokp = arg->next;
	return (0);
}
