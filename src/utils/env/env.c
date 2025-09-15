/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 02:01:32 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 02:01:36 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	split_kv(const char *line, char **out_k, char **out_v)
{
	const char	*eq;
	size_t		kl;

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

static char	*str_join3(const char *a, const char *b, const char *c)
{
	size_t	la;
	size_t	lb;
	size_t	lc;
	char	*s;

	la = 0;
	if (a)
		la = ft_strlen(a);
	lb = 0;
	if (b)
		lb = ft_strlen(b);
	lc = 0;
	if (c)
		lc = ft_strlen(c);
	s = (char *)malloc(la + lb + lc + 1);
	if (!s)
		return (NULL);
	if (a)
		ft_memcpy(s, a, la);
	if (b)
		ft_memcpy(s + la, b, lb);
	if (c)
		ft_memcpy(s + la + lb, c, lc);
	s[la + lb + lc] = '\0';
	return (s);
}

t_env	*env_from_envp(char **envp)
{
	t_env	*lst;
	int		i;
	char	*k;
	char	*v;
	t_env	*node;

	lst = NULL;
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		if (split_kv(envp[i], &k, &v) == 0)
		{
			node = env_new(k, v);
			free(k);
			free(v);
			if (!node)
			{
				free_env(&lst);
				return (NULL);
			}
			env_add_back(&lst, node);
		}
		i++;
	}
	return (lst);
}

char	**env_list_to_envp(t_env *lst)
{
	size_t	n;
	size_t	i;
	t_env	*cur;
	char	**out;
	char	*line;
	const char *valptr;

	n = 0;
	cur = lst;
	while (cur)
	{
		n++;
		cur = cur->next;
	}
	out = (char **)malloc(sizeof(char *) * (n + 1));
	if (!out)
		return (NULL);
	i = 0;
	cur = lst;
	while (cur)
	{
		valptr = cur->value;
		if (!valptr)
			valptr = "";
		line = str_join3(cur->key, "=", valptr);
		if (!line)
		{
			free_tab(out);
			return (NULL);
		}
		out[i] = line;
		i++;
		cur = cur->next;
	}
	out[i] = NULL;
	return (out);
}

