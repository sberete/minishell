/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:51:16 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 12:48:23 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#include "minishell.h"
#include <dirent.h>

/* match '*' (zéro ou plus de caractères), pas de '?' ni [] */
static int match_star(const char *name, const char *pat)
{
	while (*pat)
	{
		if (*pat == '*')
		{
			/* consommer les '*' consécutifs */
			while (*pat == '*')
				pat++;
			if (*pat == '\0')
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
		name++;
		pat++;
	}
	return (*name == '\0');
}

static int pattern_has_star(const char *s)
{
	size_t i;

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

/* retourne un tableau trié de matches, ou NULL si malloc fail
   si aucun match: renvoie tableau à 0 élément (NULL immédiatement) */
static char **glob_list_matches(const char *pat)
{
	DIR *d;
	struct dirent *ent;
	char **arr;
	size_t cap, n;
	int want_hidden;

	d = opendir(".");
	if (!d)
		return (NULL);
	cap = 16;
	n = 0;
	arr = (char **)malloc(sizeof(char *) * cap);
	if (!arr)
	{
		closedir(d);
		return (NULL);
	}
	want_hidden = (pat[0] == '.');
	while ((ent = readdir(d)) != NULL)
	{
		const char *name = ent->d_name;
		if (!want_hidden && name[0] == '.')
			continue ;
		if (match_star(name, pat))
		{
			char *dup = ft_strdup(name);
			if (!dup)
				goto oom;
			if (n + 1 >= cap)
			{
				size_t newcap = cap * 2;
				char **tmp = (char **)malloc(sizeof(char *) * newcap);
				size_t i = 0;
				if (!tmp)
				{
					free(dup);
					goto oom;
				}
				while (i < n)
				{
					tmp[i] = arr[i];
					i++;
				}
				free(arr);
				arr = tmp;
				cap = newcap;
			}
			arr[n++] = dup;
		}
	}
	closedir(d);
	/* tri insertion simple */
	{
		size_t i = 1;
		while (i < n)
		{
			char *key = arr[i];
			size_t j = i;
			while (j > 0 && ft_strcmp(arr[j - 1], key) > 0)
			{
				arr[j] = arr[j - 1];
				j--;
			}
			arr[j] = key;
			i++;
		}
	}
	arr[n] = NULL;
	return (arr);
oom:
	closedir(d);
	/* free partiel */
	{
		size_t i = 0;
		while (i < n)
		{
			free(arr[i]);
			i++;
		}
		free(arr);
	}
	return (NULL);
}

/* applique le glob sur chaque argv: si pas d’étoile ou aucun match -> on garde tel quel */
char **expand_argv_glob(char **argv)
{
	size_t i, n_out, cap;
	char **out;

	/* compter */
	i = 0;
	cap = 16;
	n_out = 0;
	out = (char **)malloc(sizeof(char *) * cap);
	if (!out)
		return (NULL);
	while (argv && argv[i])
	{
		if (pattern_has_star(argv[i]))
		{
			char **matches = glob_list_matches(argv[i]);
			if (!matches)
			{
				free_tab(out);
				return (NULL);
			}
			if (matches[0] != NULL)
			{
				size_t k = 0;
				while (matches[k])
				{
					if (n_out + 2 > cap)
					{
						size_t newcap = cap * 2;
						char **tmp = (char **)malloc(sizeof(char *) * newcap);
						size_t t = 0;
						if (!tmp)
						{
							free_tab(matches);
							free_tab(out);
							return (NULL);
						}
						while (t < n_out)
						{
							tmp[t] = out[t];
							t++;
						}
						free(out);
						out = tmp;
						cap = newcap;
					}
					out[n_out++] = ft_strdup(matches[k]);
					k++;
				}
				free_tab(matches);
			}
			else
			{
				/* aucun match -> conserver le littéral */
				if (n_out + 2 > cap)
				{
					size_t newcap = cap * 2;
					char **tmp = (char **)malloc(sizeof(char *) * newcap);
					size_t t = 0;
					if (!tmp)
					{
						free_tab(out);
						return (NULL);
					}
					while (t < n_out)
					{
						tmp[t] = out[t];
						t++;
					}
					free(out);
					out = tmp;
					cap = newcap;
				}
				out[n_out++] = ft_strdup(argv[i]);
			}
		}
		else
		{
			if (n_out + 2 > cap)
			{
				size_t newcap = cap * 2;
				char **tmp = (char **)malloc(sizeof(char *) * newcap);
				size_t t = 0;
				if (!tmp)
				{
					free_tab(out);
					return (NULL);
				}
				while (t < n_out)
				{
					tmp[t] = out[t];
					t++;
				}
				free(out);
				out = tmp;
				cap = newcap;
			}
			out[n_out++] = ft_strdup(argv[i]);
		}
		i++;
	}
	out[n_out] = NULL;
	return (out);
}

