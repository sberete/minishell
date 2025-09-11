#include "minishell.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int ft_pwd(char **argv, t_data *data)
{
	char *cwd;

	(void)argv;
	(void)data;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		ft_putstr_fd("minishell: pwd: ", STDERR_FILENO);
		ft_putstr_fd((char *)strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		return 1;
	}
	ft_putstr_fd(cwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	free(cwd);
	return 0;
}