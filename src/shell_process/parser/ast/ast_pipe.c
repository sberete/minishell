/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_pipe.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:11:54 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:23:11 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_ast	*right;
	t_token	*tok;
	t_ast	*node;

	left = parse_group(tokens);
	if (!left)
		return (NULL);
	tok = *tokens;
	while (tok && tok->type == T_PIPE)
	{
		*tokens = tok->next;
		right = parse_group(tokens);
		if (!right)
			return (free_ast(left), NULL);
		node = new_ast_node(NODE_PIPE);
		if (!node)
			return (free_ast(left), free_ast(right), NULL);
		node->left = left;
		node->right = right;
		left = node;
		tok = *tokens;
	}
	return (left);
}
