/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_group_node.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:11:44 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:23:03 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast	*parse_group(t_token **tokens)
{
	t_token	*tok;
	t_ast	*child;
	t_ast	*grp;

	tok = *tokens;
	if (!tok)
		return (NULL);
	if (tok->type != T_PAREN_OPEN)
		return (parse_command(tokens));
	*tokens = tok->next;
	child = parse_sequence(tokens);
	if (!child)
		return (NULL);
	tok = *tokens;
	if (!tok || tok->type != T_PAREN_CLOSE)
		return (free_ast(child), NULL);
	*tokens = tok->next;
	grp = new_ast_node(NODE_GROUP);
	if (!grp)
		return (free_ast(child), NULL);
	grp->child = child;
	return (grp);
}
