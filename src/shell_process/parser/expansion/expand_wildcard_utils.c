#include "minishell.h"
#include <dirent.h>

/* teste s contient au moins un '*' non vide */
int	pattern_has_star(const char *s)
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

/* match simple '*' (pas de classes), récursif */
static int	match_star_rec(const char *name, const char *pat)
{
	if (*pat == '\0')
		return (*name == '\0');
	if (*pat == '*')
	{
		while (*pat == '*')
			pat++;
		if (*pat == '\0')
			return (1);
		while (*name)
		{
			if (match_star_rec(name, pat))
				return (1);
			name++;
		}
		return (0);
	}
	if (*name != '\0' && *pat == *name)
		return (match_star_rec(name + 1, pat + 1));
	return (0);
}

/* règle des dotfiles: si motif ne commence pas par '.',
	on ne matche pas les noms '.' */
int	glob_match_name(const char *name, const char *pat)
{
	if (!name || !pat)
		return (0);
	if (name[0] == '.' && pat[0] != '.')
		return (0);
	return (match_star_rec(name, pat));
}

/* tri insertion d'un tableau de strings */
void	str_array_insertion_sort(char **arr, size_t n)
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
