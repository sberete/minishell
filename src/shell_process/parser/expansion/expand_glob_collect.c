/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_glob_collect.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:10:09 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:59:37 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	push_all_matches(char ***out, size_t *cap, size_t *n, char **m)
{
	size_t	k;

	k = 0;
	while (m[k])
	{
		if (arr_push_dup(out, cap, n, m[k]) != 0)
		{
			free_tab(m);
			return (1);
		}
		k++;
	}
	free_tab(m);
	return (0);
}

/* cas 0 match : on push le littéral */
static int	push_literal_from_nomatch(char ***out, size_t *cap,
				size_t *n, char *arg)
{
	return (arr_push_dup(out, cap, n, arg));
}

int	append_matches_or_literal(char ***out, size_t *cap, size_t *n, char *arg)
{
	char	*dir;
	char	*base;
	char	**matches;

	dir = dup_dirpath(arg, &base);
	if (!dir)
		return (1);
	matches = collect_matches(dir, base);
	free(dir);
	if (!matches)
		return (1);
	if (!matches[0])
	{
		free(matches);
		return (push_literal_from_nomatch(out, cap, n, arg));
	}
	return (push_all_matches(out, cap, n, matches));
}
