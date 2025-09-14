/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:05:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/12 08:58:22 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

/* ===================== utils quotes ===================== */

static char *strip_outer_quotes(const char *s)
{
	size_t len;

	if (!s)
		return NULL;
	len = ft_strlen(s);
	if (len >= 2 && (s[0] == '\'' || s[0] == '"') && s[len - 1] == s[0])
		return ft_substr(s, 1, len - 2);
	return ft_strdup(s);
}

/* ===================== small string builder ===================== */

static int append_mem(char **dst, const char *src, size_t len)
{
	size_t  oldlen;
	char    *out;

	if (!src || len == 0)
		return 0;
	if (!*dst)
	{
		out = (char *)malloc(len + 1);
		if (!out)
			return -1;
		ft_memcpy(out, src, len);
		out[len] = '\0';
		*dst = out;
		return 0;
	}
	oldlen = ft_strlen(*dst);
	out = (char *)malloc(oldlen + len + 1);
	if (!out)
		return -1;
	ft_memcpy(out, *dst, oldlen);
	ft_memcpy(out + oldlen, src, len);
	out[oldlen + len] = '\0';
	free(*dst);
	*dst = out;
	return 0;
}

static int append_str(char **dst, const char *src)
{
	if (!src)
		return 0;
	return append_mem(dst, src, ft_strlen(src));
}

/* ===================== $ expansion ===================== */

static int is_name_start(int c) { return (c == '_' || ft_isalpha(c)); }
static int is_name_char(int c)  { return (c == '_' || ft_isalnum(c)); }

/* Expose pour heredoc si besoin */
char *ms_expand_vars(const char *s, t_data *data)
{
	size_t  i;
	char    *out;

	if (!s)
		return NULL;
	out = NULL;
	i = 0;
	while (s[i])
	{
		if (s[i] == '$')
		{
			if (s[i + 1] == '?')
			{
				char *num = ft_itoa((int)data->last_exit);
				if (!num || append_str(&out, num) < 0)
				{
					free(num);
					free(out);
					return NULL;
				}
				free(num);
				i += 2;
				continue ;
			}
			if (is_name_start((unsigned char)s[i + 1]))
			{
				size_t j = i + 2;
				while (s[j] && is_name_char((unsigned char)s[j]))
					j++;
				{
					char *key = ft_substr(s, i + 1, j - (i + 1));
					const char *val = key ? ms_getenv(data->env, key) : NULL;
					if (val && append_str(&out, val) < 0)
					{
						free(key);
						free(out);
						return NULL;
					}
					if (!val && append_str(&out, "") < 0)
					{
						free(key);
						free(out);
						return NULL;
					}
					free(key);
					i = j;
					continue ;
				}
			}
			/* $ non suivi d’un nom → garder tel quel */
			if (append_mem(&out, &s[i], 1) < 0)
			{
				free(out);
				return NULL;
			}
			i++;
			continue ;
		}
		if (append_mem(&out, &s[i], 1) < 0)
		{
			free(out);
			return NULL;
		}
		i++;
	}
	if (!out)
		out = ft_strdup("");
	return out;
}

/* ===================== glob (wildcards) ===================== */
/* On gère * et ? ; pas de classes [] (facultatif pour passer les tests 42). */

static int has_glob_chars(const char *s)
{
	size_t i;

	if (!s)
		return 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == '*' || s[i] == '?')
			return 1;
		i++;
	}
	return 0;
}

/* Simple matcher type glob: '*' → 0..n char, '?' → 1 char */
static int match_glob(const char *name, const char *pat)
{
	if (!*pat)
		return !*name;
	if (*pat == '*')
	{
		/* essaye de consommer 0..n chars */
		while (*pat == '*')
			pat++;
		if (!*pat)
			return 1;
		while (*name)
		{
			if (match_glob(name, pat))
				return 1;
			name++;
		}
		return 0;
	}
	if (*pat == '?')
	{
		if (!*name)
			return 0;
		return match_glob(name + 1, pat + 1);
	}
	if (*pat == *name)
		return match_glob(name + 1, pat + 1);
	return 0;
}

/* concat dir + "/" + name */
static char *join_dir_name(const char *dir, const char *name)
{
	if (!dir || dir[0] == '\0' || (dir[0] == '.' && dir[1] == '\0'))
		return ft_strdup(name);
	return ft_pathjoin(dir, name);
}

/* collecte des matches pour pattern potentiellement avec répertoire */
static int collect_glob_matches(const char *pattern, char ***outv, size_t *outn)
{
	char        *dir = NULL;
	char        *base = NULL;
	const char  *bptr;
	DIR         *dp;
	struct dirent *de;
	char        **list = NULL;
	size_t      n = 0, cap = 0;
	int         include_dot = 0;

	if (!pattern)
		return 0;

	bptr = ft_strrchr(pattern, '/');
	if (bptr)
	{
		size_t dlen = (size_t)(bptr - pattern);
		dir = ft_substr(pattern, 0, dlen);
		base = ft_strdup(bptr + 1);
	}
	else
	{
		dir = ft_strdup(".");
		base = ft_strdup(pattern);
	}
	if (!dir || !base)
	{
		free(dir);
		free(base);
		return -1;
	}
	include_dot = (base[0] == '.');

	dp = opendir(dir);
	if (!dp)
	{
		free(dir);
		free(base);
		return 0; /* pas de matches, on laissera littéral */
	}

	while ((de = readdir(dp)) != NULL)
	{
		const char *nm = de->d_name;

		if (!include_dot && nm[0] == '.')
			continue;
		if (match_glob(nm, base))
		{
			char *full = join_dir_name(dir, nm);
			if (!full)
			{
				closedir(dp);
				free(dir);
				free(base);
				/* free list */
				while (n > 0) { free(list[--n]); }
				free(list);
				return -1;
			}
			if (n == cap)
			{
				size_t ncap = (cap == 0) ? 8 : cap * 2;
				char **tmp = (char **)malloc(sizeof(char *) * ncap);
				if (!tmp)
				{
					free(full);
					closedir(dp);
					free(dir);
					free(base);
					while (n > 0) { free(list[--n]); }
					free(list);
					return -1;
				}
				if (list)
				{
					size_t i = 0;
					while (i < n) { tmp[i] = list[i]; i++; }
					free(list);
				}
				list = tmp;
				cap = ncap;
			}
			list[n++] = full;
		}
	}
	closedir(dp);
	free(dir);
	free(base);

	/* tri lexicographique simple */
	if (n > 1)
	{
		size_t i = 0;
		while (i + 1 < n)
		{
			size_t j = i + 1;
			while (j < n)
			{
				if (ft_strcmp(list[i], list[j]) > 0)
				{
					char *t = list[i];
					list[i] = list[j];
					list[j] = t;
				}
				j++;
			}
			i++;
		}
	}

	*outv = list;
	*outn = n;
	return 0;
}

/* ===================== public API ===================== */

static char *expand_word_vars_then_unquote(const char *raw, t_data *data, int can_expand)
{
	char *unq;
	char *res;

	if (!raw)
		return NULL;
	unq = strip_outer_quotes(raw);
	if (!unq)
		return NULL;
	if (!can_expand)
		return unq; /* pas d’expansion si quotes simples originales */

	res = ms_expand_vars(unq, data);
	free(unq);
	return res;
}

/* argv: expansion $ + glob (*) ; respecte argv_can_expand pour quotes simples */
char **expand_argv_dup(t_ast *cmd, t_data *data)
{
	size_t  i;
	size_t  argc;
	char  **out = NULL;
	size_t  n = 0, cap = 0;

	if (!cmd || !cmd->argv)
		return NULL;

	argc = 0;
	while (cmd->argv[argc])
		argc++;

	i = 0;
	while (i < argc)
	{
		int can = 1;
		char *expanded;
		if (cmd->argv_can_expand)
			can = cmd->argv_can_expand[i] ? 1 : 0;

		expanded = expand_word_vars_then_unquote(cmd->argv[i], data, can);
		if (!expanded)
		{
			/* free out */
			while (n > 0) { free(out[--n]); }
			free(out);
			return NULL;
		}

		if (can && has_glob_chars(expanded))
		{
			char **matches = NULL;
			size_t m = 0;
			if (collect_glob_matches(expanded, &matches, &m) < 0)
			{
				free(expanded);
				while (n > 0) { free(out[--n]); }
				free(out);
				return NULL;
			}
			if (m > 0)
			{
				size_t k = 0;
				while (k < m)
				{
					if (n == cap)
					{
						size_t ncap = (cap == 0) ? 8 : cap * 2;
						char **tmp = (char **)malloc(sizeof(char *) * ncap);
						if (!tmp)
						{
							/* free matches */
							size_t t = 0;
							while (t < m) { free(matches[t++]); }
							free(matches);
							free(expanded);
							while (n > 0) { free(out[--n]); }
							free(out);
							return NULL;
						}
						if (out)
						{
							size_t t = 0;
							while (t < n) { tmp[t] = out[t]; t++; }
							free(out);
						}
						out = tmp;
						cap = ncap;
					}
					out[n++] = matches[k++];
				}
				free(matches);
				free(expanded);
				i++;
				continue;
			}
			/* pas de match -> garder littéral */
			if (matches)
				free(matches);
		}

		/* append expanded tel quel */
		if (n == cap)
		{
			size_t ncap = (cap == 0) ? 8 : cap * 2;
			char **tmp = (char **)malloc(sizeof(char *) * ncap);
			if (!tmp)
			{
				free(expanded);
				while (n > 0) { free(out[--n]); }
				free(out);
				return NULL;
			}
			if (out)
			{
				size_t t = 0;
				while (t < n) { tmp[t] = out[t]; t++; }
				free(out);
			}
			out = tmp;
			cap = ncap;
		}
		out[n++] = expanded;
		i++;
	}

	/* terminer */
	if (n == cap)
	{
		char **tmp = (char **)malloc(sizeof(char *) * (cap + 1));
		if (!tmp)
		{
			while (n > 0) { free(out[--n]); }
			free(out);
			return NULL;
		}
		if (out)
		{
			size_t t = 0;
			while (t < n) { tmp[t] = out[t]; t++; }
			free(out);
		}
		out = tmp;
		cap = n; /* pas important ensuite */
	}
	out[n] = NULL;
	return out;
}

/* redirections: $ + glob; si plusieurs matches -> ambiguous redirect */
int expand_redirs_inplace(t_redir *r, t_data *data)
{
	while (r)
	{
		if (r->type == REDIR_IN || r->type == REDIR_OUT || r->type == REDIR_APPEND)
		{
			char *expanded = expand_word_vars_then_unquote(
				r->filename, data, r->filename_can_expand ? 1 : 0);
			if (!expanded)
				return 1;

			if ((r->filename_can_expand ? 1 : 0) && has_glob_chars(expanded))
			{
				char  **matches = NULL;
				size_t  m = 0;
				int     rc = collect_glob_matches(expanded, &matches, &m);
				if (rc < 0)
				{
					free(expanded);
					return 1;
				}
				if (m > 1)
				{
					size_t i = 0;
					dprintf(2, "minishell: %s: ambiguous redirect\n", expanded);
					while (i < m) { free(matches[i++]); }
					free(matches);
					free(expanded);
					return 1;
				}
				if (m == 1)
				{
					free(r->filename);
					r->filename = matches[0];
					free(matches);
					free(expanded);
				}
				else
				{
					/* 0 match → garder littéral */
					free(r->filename);
					r->filename = expanded;
					if (matches)
						free(matches);
				}
			}
			else
			{
				free(r->filename);
				r->filename = expanded;
			}
		}
		/* HEREDOC: on ne touche pas r->delim ici */
		r = r->next;
	}
	return 0;
}
