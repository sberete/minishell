/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_array.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:08:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:16:06 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* --- tableau dynamique de strings (NULL-terminé à la fin) --- */
static size_t	next_cap(size_t cap)
{
	if (cap == 0)
		return (8);
	return (cap * 2);
}

/* --- tableau dynamique de strings (NULL-terminé à la fin) --- */
int	arr_grow(char ***arr, size_t *cap)
{
	char	**tmp;
	size_t	i;
	size_t	newcap;

	newcap = next_cap(*cap);
	tmp = (char **)calloc(newcap, sizeof(char *));
	if (!tmp)
		return (1);
	i = 0;
	if (arr && *arr)
	{
		while (i < *cap && (*arr)[i])
		{
			tmp[i] = (*arr)[i];
			i++;
		}
	}
	free(*arr);
	*arr = tmp;
	*cap = newcap;
	return (0);
}

int	arr_push_dup(char ***arr, size_t *cap, size_t *n, char *s)
{
	char		*dup;
	const char	*src;

	if (*n + 2 > *cap)
	{
		if (arr_grow(arr, cap) != 0)
			return (1);
	}
	if (s == NULL)
		src = "";
	else
		src = s;
	dup = ft_strdup(src);
	if (!dup)
		return (1);
	(*arr)[*n] = dup;
	*n = *n + 1;
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
