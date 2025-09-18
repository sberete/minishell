/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:51:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:59:43 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you                                           +#+  +:+
	+#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:30:00 by you               #+#    #+#             */
/*   Updated: 2025/09/18 05:30:00 by you              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redir_tok(t_token_type t)
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

int	is_binop_tok(t_token_type t)
{
	if (t == T_AND)
		return (1);
	if (t == T_OR)
		return (1);
	if (t == T_PIPE)
		return (1);
	return (0);
}

int	can_start_cmd_tok(t_token_type t)
{
	if (t == T_WORD)
		return (1);
	if (t == T_PAREN_OPEN)
		return (1);
	if (is_redir_tok(t))
		return (1);
	return (0);
}

int	is_ctrl_token_tok(t_token_type t)
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
