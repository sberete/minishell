#include "minishell.h"

t_data	data_init(int argc, char **argv, char **envp)
{
	t_data d;

	(void)argc;
	(void)argv;
	ft_bzero(&d, sizeof(t_data));
	d.env = env_from_environ(envp);
	d.last_exit = 0;
	return (d);
}