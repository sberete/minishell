/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:53:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/14 17:31:23 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Retourne 1 si c est le début d'un opérateur shell */
int	is_operator_start(char c)
{
	if (c == '|' || c == '&' || c == '<' || c == '>'
		|| c == ';' || c == '(' || c == ')')
		return (1);
	return (0);
}

/* Longueur de l'opérateur à partir de str (0 si aucun) */
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
