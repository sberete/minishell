/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_glob_match.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:08:54 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:09:11 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* '*' only, no '?', '[]' ; récursif mais court */
int	match_star(char *name, char *pat)
{
	if (!*pat)
		return (*name == '\0');
	if (*pat == '*')
	{
		while (*pat == '*')
			pat++;
		if (!*pat)
			return (1);
		while (*name)
		{
			if (match_star(name, pat))
				return (1);
			name++;
		}
		return (0);
	}
	if (*name != *pat)
		return (0);
	return (match_star(name + 1, pat + 1));
}

int	pattern_has_star(char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] == '*')
			return (1);
		i++;
	}
	return (0);
}
