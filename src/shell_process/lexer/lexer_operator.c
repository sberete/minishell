/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:53:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 22:17:34 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_operator_start(char c)
{
	if (c == '|' || c == '&' || c == '<' || c == '>' || c == ';' || c == '('
		|| c == ')')
		return (1);
	return (0);
}

int	get_operator_len(const char *str)
{
	if (!str || !*str)
		return (0);
	if (str[1])
	{
		if (ft_strncmp(str, "&&", 2) == 0 || ft_strncmp(str, "||", 2) == 0
			|| ft_strncmp(str, ">>", 2) == 0 || ft_strncmp(str, "<<", 2) == 0)
			return (2);
	}
	if (is_operator_start(str[0]))
		return (1);
	return (0);
}
