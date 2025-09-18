/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:11:46 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:22:48 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_ctrl_token(t_token_type t)
{
	if (t == T_PIPE)
		return (1);
	if (t == T_AND)
		return (1);
	if (t == T_OR)
		return (1);
	if (t == T_SEPARATOR)
		return (1);
	if (t == T_PAREN_CLOSE)
		return (1);
	if (t == T_END)
		return (1);
	return (0);
}

/* essaie d’avancer d’un élément (WORD ou REDIR) ; -1=err, 0=stop, 1=ok */
static int	step_cmd(t_ast *cmd, t_token **tp)
{
	t_token	*t;

	t = *tp;
	if (!t)
		return (0);
	if (t->type == T_WORD)
	{
		if (!add_arg(cmd, t->value, t->can_expand))
			return (-1);
		*tp = t->next;
		return (1);
	}
	if (t->type == T_REDIR_IN || t->type == T_REDIR_OUT
		|| t->type == T_APPEND || t->type == T_HEREDOC)
	{
		if (parse_redirection(cmd, tp) != 0)
			return (-1);
		return (1);
	}
	return (0);
}

t_ast	*parse_command(t_token **tokp)
{
	t_ast	*cmd;
	t_token	*t;
	int		rc;

	if (!tokp || !*tokp)
		return (NULL);
	cmd = new_ast_node(NODE_CMD);
	if (!cmd)
		return (NULL);
	t = *tokp;
	while (t && !is_ctrl_token(t->type))
	{
		rc = step_cmd(cmd, &t);
		if (rc == -1 || rc == 0)
			return (free_ast(cmd), NULL);
	}
	*tokp = t;
	return (cmd);
}
