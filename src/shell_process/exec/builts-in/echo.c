#include "minishell.h"

static int is_n_flag(const char *s)
{
	size_t i;

	if (!s)
		return 0;
	if (s[0] != '-')
		return 0;
	if (s[1] == '\0')
		return 0;
	i = 1;
	while (s[i] == 'n')
		i++;
	if (s[i] != '\0')
		return 0;
	return 1;
}

int ft_echo(char **argv, t_data *data)
{
	int i;
	int newline;

	(void)data;
	i = 1;
	newline = 1;
	while (argv[i] && is_n_flag(argv[i]))
	{
		newline = 0;
		i++;
	}
	while (argv[i])
	{
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		if (argv[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return 0;
}