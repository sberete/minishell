#include "minishell.h"

t_data	data_init(int argc, char **argv, char **env)
{
	t_data d;

	(void)argc;
	(void)argv;
	ft_bzero(&d, sizeof(t_data));
	d.env = env_from_environ(env);
	d.last_exit = 0;
	return (d);
}