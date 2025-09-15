/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 01:42:05 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 02:18:41 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	env_count(t_env *lst)
{
	size_t	n;

	n = 0;
	while (lst)
	{
		n++;
		lst = lst->next;
	}
	return (n);
}

static void	env_array_sort(t_env **arr, size_t n)
{
	size_t	i;
	size_t	j;
	t_env	*key;

	i = 1;
	while (i < n)
	{
		key = arr[i];
		j = i;
		while (j > 0 && ft_strcmp(arr[j - 1]->key, key->key) > 0)
		{
			arr[j] = arr[j - 1];
			j--;
		}
		arr[j] = key;
		i++;
	}
}

void	export_print_sorted(t_env *lst)
{
	size_t	n;
	size_t	i;
	t_env	*cur;
	t_env	**arr;

	n = env_count(lst);
	if (n == 0)
		return ;
	arr = malloc(sizeof(t_env *) * n);
	if (!arr)
		return ;
	i = 0;
	cur = lst;
	while (cur)
	{
		arr[i] = cur;
		i++;
		cur = cur->next;
	}
	env_array_sort(arr, n);
	i = 0;
	while (i < n)
	{
		printf("declare -x %s", arr[i]->key);
		if (arr[i]->value)
			printf("=\"%s\"", arr[i]->value);
		printf("\n");
		i++;
	}
	free(arr);
}

static int	env_append_local(t_env **lst, char *key, char *suffix)
{
	t_env	*cur;
	char	*joined;

	if (!lst || !key || !suffix)
		return (-1);
	cur = env_find(*lst, key);
	if (!cur)
		return (env_set(lst, key, suffix, 1));
	if (cur->value)
		joined = ft_strjoin(cur->value, suffix);
	else
		joined = ft_strdup(suffix);
	if (!joined)
		return (-1);
	free(cur->value);
	cur->value = joined;
	return (0);
}

int	export_set_assignment(t_env **lst, char *assign)
{
	char	*key;
	char	*val;
	int		append;
	int		rc;

	if (parse_assignment(assign, &key, &val, &append) != 0)
		return (1);
	if (append)
		rc = env_append_local(lst, key, val);
	else
		rc = env_set(lst, key, val, 1);
	free(key);
	free(val);
	if (rc != 0)
		return (1);
	return (0);
}
