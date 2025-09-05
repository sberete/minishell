#include "minishell.h"

char	*read_input(t_data *data)
{
	char *line;
    (void)data;
	line = readline("minishell > ");
	if (!line)
	{
		printf("exit\n");
		rl_clear_history();
		return (NULL);
	}
	if (*line)
		add_history(line);
	return (line);
}