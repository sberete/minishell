/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 04:55:02 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 18:13:25 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	env_array_insertion_sort(t_env **arr, size_t n)
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

static void	export_print_one(t_env *e, int fd)
{
	ft_putstr_fd("declare -x ", fd);
	ft_putstr_fd(e->key, fd);
	if (e->value)
	{
		ft_putstr_fd("=\"", fd);
		ft_putstr_fd(e->value, fd);
		ft_putstr_fd("\"\n", fd);
	}
	else
		ft_putstr_fd("\n", fd);
}

static int	fill_env_ptr_array(t_env *lst, t_env **arr, size_t n)
{
	size_t	i;

	i = 0;
	while (lst && i < n)
	{
		arr[i] = lst;
		i++;
		lst = lst->next;
	}
	return (0);
}

/* PUBLIC: imprime les variables triées vers fd ; 0=ok, 1=alloc fail */
int	export_print_sorted_fd(t_env *lst, int fd)
{
	size_t	n;
	size_t	i;
	t_env	**arr;

	n = env_list_size(lst);
	if (n == 0)
		return (0);
	arr = (t_env **)malloc(sizeof(t_env *) * n);
	if (!arr)
		return (1);
	if (fill_env_ptr_array(lst, arr, n) != 0)
	{
		free(arr);
		return (1);
	}
	env_array_insertion_sort(arr, n);
	i = 0;
	while (i < n)
	{
		export_print_one(arr[i], fd);
		i++;
	}
	free(arr);
	return (0);
}

void	export_print_sorted(t_env *lst)
{
	(void)export_print_sorted_fd(lst, STDOUT_FILENO);
}
