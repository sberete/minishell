#include "minishell.h"
#include <dirent.h>

/* ----- split dir/base pour le motif dir + * ----- */

static char	*dup_dirpath(const char *pat, const char **base)
{
	const char	*slash;

	*base = pat;
	if (!pat)
		return (ft_strdup("."));
	slash = ft_strrchr(pat, '/');
	if (!slash)
		return (ft_strdup("."));
	*base = slash + 1;
	if (slash == pat)
		return (ft_strdup("/"));
	return (ft_substr(pat, 0, (size_t)(slash - pat)));
}

static int	want_hidden_first(const char *basepat)
{
	return (basepat && basepat[0] == '.');
}

/* utilise ft_join_sep(a, "/", b) */
static char	*join_dir_name(const char *dir, const char *name)
{
	if (!dir || dir[0] == '\0')
		return (ft_join_sep(".", "/", name));
	if (dir[0] == '/' && dir[1] == '\0')
		return (ft_join_sep("/", "", name));
	return (ft_join_sep(dir, "/", name));
}

/* ----- collecte des matches dans un répertoire ----- */

static char	**empty_vec(void)
{
	char	**v;

	v = (char **)malloc(sizeof(char *));
	if (!v)
		return (NULL);
	v[0] = NULL;
	return (v);
}

static int	push_match(char ***arr, size_t *cap, size_t *n, char *joined)
{
	int	rc;

	rc = arr_push_dup(arr, cap, n, joined);
	free(joined);
	return (rc);
}

static char	**collect_matches(const char *dir, const char *basepat)
{
	DIR				*d;
	struct dirent	*ent;
	char			**arr;
	size_t			cap;
	size_t			n;
	int				want_hidden;

	d = opendir(dir ? dir : ".");
	if (!d)
		return (empty_vec());
	arr = NULL;
	cap = 0;
	n = 0;
	want_hidden = want_hidden_first(basepat);
	while ((ent = readdir(d)) != NULL)
	{
		if (!want_hidden && ent->d_name[0] == '.')
			continue ;
		if (!match_star(ent->d_name, basepat))
			continue ;
		if (!arr && arr_grow(&arr, &cap) != 0)
		{
			closedir(d);
			return (NULL);
		}
		if (push_match(&arr, &cap, &n, join_dir_name(dir, ent->d_name)) != 0)
		{
			closedir(d);
			arr_free_all(arr, n);
			return (NULL);
		}
	}
	closedir(d);
	if (!arr)
		return (empty_vec());
	sort_strings(arr, n);
	return (arr);
}

/* ----- applique le glob sur un argument ----- */

static int	append_matches_or_literal(char ***out, size_t *cap, size_t *n,
		const char *arg)
{
	char		*dir;
	const char	*base;
	char		**matches;
	size_t		k;

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
		return (arr_push_dup(out, cap, n, arg));
	}
	k = 0;
	while (matches[k])
	{
		if (arr_push_dup(out, cap, n, matches[k]) != 0)
		{
			free_tab(matches);
			return (1);
		}
		k++;
	}
	free_tab(matches);
	return (0);
}

/* ----- public: expand argv avec '*' (sans '?', '[]') ----- */

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
			{
				arr_free_all(out, n);
				return (NULL);
			}
		}
		else
		{
			if (arr_push_dup(&out, &cap, &n, argv[i]) != 0)
			{
				arr_free_all(out, n);
				return (NULL);
			}
		}
		i++;
	}
	out[n] = NULL;
	return (out);
}
