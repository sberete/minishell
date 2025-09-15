/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:40:19 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 22:20:06 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	set_env_key(t_data *d, char *k, char *v)
{
	if (env_set(&d->env, k, v ? v : "", 1) != 0)
		return (1);
	return (0);
}

static int	update_pwds(t_data *d, char *oldpwd)
{
	char	*newpwd;
	int		rc;

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (1);
	rc = set_env_key(d, "OLDPWD", oldpwd);
	if (rc == 0)
		rc = set_env_key(d, "PWD", newpwd);
	free(newpwd);
	return (rc);
}

static int	cd_to_path(t_data *d, char *path)
{
	char	*oldpwd;

	oldpwd = getcwd(NULL, 0);
	if (!path || chdir(path) != 0)
	{
		if (oldpwd)
			free(oldpwd);
		builtin_err_arg("cd", path, "No such file or directory");
		return (1);
	}
	if (update_pwds(d, oldpwd) != 0)
	{
		if (oldpwd)
			free(oldpwd);
		return (1);
	}
	if (oldpwd)
		free(oldpwd);
	return (0);
}

static int	cd_dash(t_data *d)
{
	char	*old;

	old = env_get(d->env, "OLDPWD");
	if (!old || *old == '\0')
		return (builtin_err("cd", "OLDPWD not set"), 1);
	ft_putstr_fd(old, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	return (cd_to_path(d, old));
}

int	builtin_cd(t_data *d, char **av)
{
	char	*home;

	if (!av || !av[1] || av[1][0] == '\0')
	{
		home = env_get(d->env, "HOME");
		if (!home || *home == '\0')
			return (builtin_err("cd", "HOME not set"), 1);
		return (cd_to_path(d, home));
	}
	if (ft_strcmp(av[1], "-") == 0)
		return (cd_dash(d));
	return (cd_to_path(d, av[1]));
}
