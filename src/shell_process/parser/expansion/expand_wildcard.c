/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:51:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 15:12:20 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <dirent.h>

/* renvoie un tableau trié des matches du motif dans le répertoire courant */
static char	**glob_collect_matches(const char *pat, size_t *out_n)
{
	DIR				*d;
	struct dirent	*e;
	char			**arr;
	size_t			cap;
	size_t			n;
	char			*dup;

	*out_n = 0;
	d = opendir(".");
	if (!d)
		return (NULL);
	cap = 8;
	n = 0;
	arr = (char **)malloc(sizeof(char *) * cap);
	if (!arr)
	{
		closedir(d);
		return (NULL);
	}
	e = readdir(d);
	while (e)
	{
		if (glob_match_name(e->d_name, pat))
		{
			if (n == cap)
			{
				char **tmp = (char **)malloc(sizeof(char *) * (cap * 2));
				if (!tmp)
					break ;
				ft_memcpy(tmp, arr, sizeof(char *) * n);
				free(arr);
				arr = tmp;
				cap = cap * 2;
			}
			dup = ft_strdup(e->d_name);
			if (!dup)
				break ;
			arr[n] = dup;
			n++;
		}
		e = readdir(d);
	}
	closedir(d);
	*out_n = n;
	if (n == 0)
	{
		free(arr);
		return (NULL);
	}
	str_array_insertion_sort(arr, n);
	return (arr);
}

/* expand glob sur argv: remplace chaque motif par ses matches (ou le garde) */
char	**expand_argv_glob(char **argv)
{
	size_t	i;
	size_t	n_in;
	size_t	n_out;
	char	**out;
	size_t	cap;
	char	**matches;
	size_t	mc;

	if (!argv)
		return (NULL);
	n_in = 0;
	while (argv[n_in])
		n_in++;
	cap = n_in + 8;
	out = (char **)malloc(sizeof(char *) * cap);
	if (!out)
		return (NULL);
	n_out = 0;
	i = 0;
	while (i < n_in)
	{
		matches = NULL;
		mc = 0;
		if (pattern_has_star(argv[i]))
			matches = glob_collect_matches(argv[i], &mc);
		if (!matches)
		{
			if (n_out + 1 >= cap)
			{
				char **tmp = (char **)malloc(sizeof(char *) * (cap * 2));
				if (!tmp)
					return (/* leak minimal */ out);
				ft_memcpy(tmp, out, sizeof(char *) * n_out);
				free(out);
				out = tmp;
				cap = cap * 2;
			}
			out[n_out] = ft_strdup(argv[i]);
			if (!out[n_out])
				return (out);
			n_out++;
		}
		else
		{
			size_t k = 0;
			while (k < mc)
			{
				if (n_out + 1 >= cap)
				{
					char **tmp2 = (char **)malloc(sizeof(char *) * (cap * 2));
					if (!tmp2)
						return (out);
					ft_memcpy(tmp2, out, sizeof(char *) * n_out);
					free(out);
					out = tmp2;
					cap = cap * 2;
				}
				out[n_out] = matches[k];
				n_out++;
				k++;
			}
			free(matches);
		}
		i++;
	}
	out[n_out] = NULL;
	return (out);
}
