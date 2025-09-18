/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 23:01:40 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 01:51:58 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_executable(const char *path)
{
	struct stat	st;

	if (!path)
		return (0);
	if (stat(path, &st) != 0)
		return (0);
	if (!S_ISREG(st.st_mode))
		return (0);
	return (access(path, X_OK) == 0);
}

static char	**env_path_split(t_env *env)
{
	char	*p;

	p = env_get(env, "PATH");
	if (!p)
		return (NULL);
	return (ft_split(p, ':'));
}

static char	*mk_tmp(char *dir, char *name)
{
	if (!dir || dir[0] == '\0')
		return (ft_join_sep(".", "/", name));
	return (ft_join_sep(dir, "/", name));
}

static char	*search_in_paths(char **paths, char *name)
{
	size_t	i;
	char	*tmp;

	i = 0;
	while (paths[i])
	{
		tmp = mk_tmp(paths[i], name);
		if (tmp && is_executable(tmp))
			return (tmp);
		free(tmp);
		i++;
	}
	return (NULL);
}

char	*find_cmd_path(char *name, t_env *env)
{
	char	**paths;
	char	*ret;

	if (!name || *name == '\0')
		return (NULL);
	if (ft_strchr(name, '/'))
	{
		if (is_executable(name))
			return (ft_strdup(name));
		return (NULL);
	}
	paths = env_path_split(env);
	if (!paths)
		return (NULL);
	ret = search_in_paths(paths, name);
	free_tab(paths);
	return (ret);
}
