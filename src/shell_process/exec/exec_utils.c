/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 20:06:50 by sberete           #+#    #+#             */
/*   Updated: 2025/09/02 20:06:51 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**get_env_path(char **env)
{
	int		i;
	char	*path;

	i = 0;
	path = NULL;
	while (env && env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
		{
			path = env[i] + 5;
			break ;
		}
		i++;
	}
	if (!path)
		return (NULL);
	return (ft_split(path, ':'));
}

char	*resolve_path(char *cmd, char **env)
{
	char	**paths;
	char	*full;
	int		i;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	paths = get_env_path(env);
	i = 0;
	while (paths && paths[i])
	{
		full = ft_pathjoin(paths[i], cmd);
		if (access(full, X_OK) == 0)
			return (free_tab(paths), (full));
		free(full);
		i++;
	}
	free_tab(paths);
	return (NULL);
}
