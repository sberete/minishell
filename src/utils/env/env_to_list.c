/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_to_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 01:57:44 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 02:04:18 by sberete          ###   ########.fr       */
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

static char	*env_line_dup(t_env *e)
{
	char	*val;

	val = e->value;
	if (!val)
		val = "";
	return (ft_join_sep(e->key, "=", val));
}

static int	env_fill_envp(char **out, t_env *lst)
{
	size_t	i;
	char	*line;

	i = 0;
	while (lst)
	{
		line = env_line_dup(lst);
		if (!line)
		{
			out[i] = NULL;
			return (-1);
		}
		out[i] = line;
		i++;
		lst = lst->next;
	}
	out[i] = NULL;
	return (0);
}

char	**env_list_to_envp(t_env *lst)
{
	char	**out;
	size_t	n;

	n = env_count(lst);
	out = (char **)malloc(sizeof(char *) * (n + 1));
	if (!out)
		return (NULL);
	if (env_fill_envp(out, lst) != 0)
	{
		free_tab(out);
		return (NULL);
	}
	return (out);
}
