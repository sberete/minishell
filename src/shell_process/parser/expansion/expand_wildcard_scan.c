/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard_scan.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:56:27 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:56:43 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**empty_vec(void)
{
	char	**v;

	v = (char **)malloc(sizeof(char *));
	if (!v)
		return (NULL);
	v[0] = NULL;
	return (v);
}

static int	arr_push_joined(char ***arr, size_t *cap, size_t *n, char *joined)
{
	if (!*arr && arr_grow(arr, cap) != 0)
	{
		free(joined);
		return (1);
	}
	if (arr_push_dup(arr, cap, n, joined) != 0)
	{
		free(joined);
		return (1);
	}
	free(joined);
	return (0);
}

static int	should_skip_entry(int want_hidden, const struct dirent *ent)
{
	if (!want_hidden && ent->d_name[0] == '.')
		return (1);
	return (0);
}

static char	**collect_loop(DIR *d, char *dir, char *base)
{
	char			**arr;
	size_t			cap;
	size_t			n;
	struct dirent	*ent;
	int				want;

	arr = NULL;
	cap = 0;
	n = 0;
	want = want_hidden_first(base);
	while (1)
	{
		ent = readdir(d);
		if (!ent)
			break ;
		if (should_skip_entry(want, ent))
			continue ;
		if (match_star(ent->d_name, base))
		{
			if (arr_push_joined(&arr, &cap, &n, join_dir_name(dir,
						ent->d_name)) != 0)
				return (arr_free_all(arr, n), NULL);
		}
	}
	return (arr);
}

char	**collect_matches(char *dir, char *basepat)
{
	DIR		*d;
	char	**arr;
	size_t	len;
	char	*scan;

	scan = dir;
	if (!scan)
		scan = ".";
	d = opendir(scan);
	if (!d)
		return (empty_vec());
	arr = collect_loop(d, dir, basepat);
	closedir(d);
	if (!arr)
		return (empty_vec());
	len = ft_tablen(arr);
	sort_strings(arr, len);
	return (arr);
}
