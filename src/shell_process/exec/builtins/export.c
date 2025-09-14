/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:46:14 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <unistd.h>

/* impression style bash: declare -x KEY="value" (sans échappes avancées) */

typedef struct s_kvref {
	t_env *ref;
} t_kvref;

static void sort_env_by_key(t_kvref *arr, size_t n)
{
	size_t i;
	size_t j;
	t_kvref tmp;

	if (!arr || n < 2)
		return;
	i = 0;
	while (i + 1 < n)
	{
		j = i + 1;
		while (j < n)
		{
			if (ft_strcmp(arr[i].ref->key, arr[j].ref->key) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static int print_export_no_args(t_env *env)
{
	size_t n;
	size_t i;
	t_env *it;
	t_kvref *arr;

	n = env_size(env);
	arr = (t_kvref *)malloc(sizeof(t_kvref) * n);
	if (!arr)
		return 1;
	i = 0;
	it = env;
	while (it)
	{
		arr[i].ref = it;
		it = it->next;
		i++;
	}
	sort_env_by_key(arr, n);
	i = 0;
	while (i < n)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(arr[i].ref->key, STDOUT_FILENO);
		if (arr[i].ref->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(arr[i].ref->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		i++;
	}
	free(arr);
	return 0;
}

int ft_export(char **argv, t_data *data)
{
	int i;
	int status;

	if (!argv[1])
		return print_export_no_args(data->env);

	status = 0;
	i = 1;
	while (argv[i])
	{
		if (ft_strchr(argv[i], '=') == NULL)
		{
			/* export KEY (sans =) : juste marquer comme exporté (dans 42 souvent on le garde) */
			if (!id_is_valid(argv[i]))
			{
				ft_putstr_fd("minishell: export: `", STDERR_FILENO);
				ft_putstr_fd(argv[i], STDERR_FILENO);
				ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
				status = 1;
			}
			else
			{
				/* Si la clé n’existe pas, l’ajouter avec value NULL */
				if (env_find(data->env, argv[i]) == NULL)
					ms_setenv(&data->env, argv[i], NULL, 0);
			}
			i++;
			continue;
		}

		/* cas KEY=value ou KEY+=value */
		if (ms_setenv_append(&data->env, argv[i]) == 0)
		{
			/* c'était KEY+=VAL, déjà géré */
			i++;
			continue;
		}
		/* sinon KEY=VAL (ms_setenv_eq fait le split et set) */
		if (ms_setenv_eq(&data->env, argv[i]) != 0)
		{
			/* identifiant invalide */
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			status = 1;
		}
		i++;
	}
	return status;
}
