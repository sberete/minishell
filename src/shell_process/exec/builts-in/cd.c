#include "minishell.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static int set_pwd_vars(t_data *data, const char *oldpwd)
{
	char *cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		/* Si getcwd échoue, on essaie quand même de mettre OLDPWD */
		if (oldpwd)
			ms_setenv(&data->env, "OLDPWD", oldpwd, 1);
		return 0;
	}
	if (oldpwd)
		ms_setenv(&data->env, "OLDPWD", oldpwd, 1);
	ms_setenv(&data->env, "PWD", cwd, 1);
	free(cwd);
	return 0;
}

int ft_cd(char **argv, t_data *data)
{
	const char *target;
	const char *home;
	char       *oldpwd;
	int         ac;

	ac = 0;
	while (argv[ac])
		ac++;
	if (ac > 2)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return 1;
	}
	if (ac == 1)
	{
		home = ms_getenv(data->env, "HOME");
		if (!home || home[0] == '\0')
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
			return 1;
		}
		target = home;
	}
	else
		target = argv[1];

	oldpwd = getcwd(NULL, 0);
	if (chdir(target) != 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd((char *)target, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd((char *)strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		if (oldpwd)
			free(oldpwd);
		return 1;
	}
	set_pwd_vars(data, oldpwd);
	if (oldpwd)
		free(oldpwd);
	return 0;
}