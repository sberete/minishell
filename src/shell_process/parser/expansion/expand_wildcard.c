/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:48:05 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 02:02:15 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* push chaque match, free en cas d'erreur */
static int	push_matches(char ***out, size_t *cap, size_t *n, char **m)
{
	size_t	k;

	k = 0;
	while (m[k])
	{
		if (arr_push_dup(out, cap, n, m[k]) != 0)
		{
			arr_free_all(m, k);
			return (1);
		}
		k++;
	}
	arr_free_all(m, k);
	return (0);
}

static int	append_matches_or_literal(char ***out, size_t *cap, size_t *n,
		char *arg)
{
	char	*dir;
	char	*base;
	char	**m;

	dir = dup_dirpath(arg, &base);
	if (!dir)
		return (1);
	m = collect_matches(dir, base);
	free(dir);
	if (!m)
		return (1);
	if (!m[0])
	{
		arr_free_all(m, 0);
		return (arr_push_dup(out, cap, n, arg));
	}
	return (push_matches(out, cap, n, m));
}

char	**expand_argv_glob(char **argv)
{
	size_t	i;
	size_t	n;
	size_t	cap;
	char	**out;

	i = 0;
	n = 0;
	cap = 0;
	out = NULL;
	if (arr_grow(&out, &cap) != 0)
		return (NULL);
	while (argv && argv[i])
	{
		if (pattern_has_star(argv[i]))
		{
			if (append_matches_or_literal(&out, &cap, &n, argv[i]) != 0)
				return (arr_free_all(out, n), NULL);
		}
		else if (arr_push_dup(&out, &cap, &n, argv[i]) != 0)
			return (arr_free_all(out, n), NULL);
		i++;
	}
	out[n] = NULL;
	return (out);
}
