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

/* --- tableau dynamique de strings (NULL-terminé à la fin) --- */

int	arr_grow(char ***arr, size_t *cap)
{
	char	**tmp;
	size_t	i;
	size_t	newcap;

	newcap = (*cap == 0) ? 8 : (*cap * 2);
	tmp = (char **)malloc(sizeof(char *) * newcap);
	if (!tmp)
		return (1);
	i = 0;
	while (i < *cap && *arr && (*arr)[i])
	{
		tmp[i] = (*arr)[i];
		i++;
	}
	while (i < newcap)
	{
		tmp[i] = NULL;
		i++;
	}
	free(*arr);
	*arr = tmp;
	*cap = newcap;
	return (0);
}

int	arr_push_dup(char ***arr, size_t *cap, size_t *n, char *s)
{
	char	*dup;

	if (*n + 2 > *cap)
	{
		if (arr_grow(arr, cap) != 0)
			return (1);
	}
	dup = ft_strdup(s ? s : "");
	if (!dup)
		return (1);
	(*arr)[*n] = dup;
	*n += 1;
	(*arr)[*n] = NULL;
	return (0);
}

void	arr_free_all(char **arr, size_t n)
{
	size_t	i;

	if (!arr)
		return ;
	i = 0;
	while (i < n && arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/* insertion sort (petit n) */
void	sort_strings(char **arr, size_t n)
{
	size_t	i;
	size_t	j;
	char	*key;

	i = 1;
	while (i < n)
	{
		key = arr[i];
		j = i;
		while (j > 0 && ft_strcmp(arr[j - 1], key) > 0)
		{
			arr[j] = arr[j - 1];
			j--;
		}
		arr[j] = key;
		i++;
	}
}
