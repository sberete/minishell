/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_to_tab.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 01:57:42 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 02:04:13 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	split_kv(char *line, char **out_k, char **out_v)
{
	char	*eq;
	size_t	kl;

	if (!line || !out_k || !out_v)
		return (-1);
	eq = ft_strchr(line, '=');
	if (!eq)
		return (-1);
	kl = (size_t)(eq - line);
	*out_k = (char *)malloc(kl + 1);
	if (!*out_k)
		return (-1);
	ft_memcpy(*out_k, line, kl);
	(*out_k)[kl] = '\0';
	*out_v = ft_strdup(eq + 1);
	if (!*out_v)
	{
		free(*out_k);
		return (-1);
	}
	return (0);
}

static int	env_push_line(t_env **lst, char *line)
{
	char	*k;
	char	*v;
	t_env	*node;

	if (split_kv(line, &k, &v) != 0)
		return (0);
	node = env_new(k, v);
	free(k);
	free(v);
	if (!node)
		return (-1);
	env_add_back(lst, node);
	return (0);
}

t_env	*env_from_envp(char **envp)
{
	t_env	*lst;
	int		i;

	lst = NULL;
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		if (env_push_line(&lst, envp[i]) != 0)
		{
			free_env(&lst);
			return (NULL);
		}
		i++;
	}
	return (lst);
}
