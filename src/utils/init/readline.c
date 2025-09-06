#include "minishell.h"

int read_input(t_data *data)
{
	data->line = readline("minishell > ");
	if (data->line == NULL)
	{
		printf("exit\n");
		rl_clear_history();
		return (0);
	}
	if (data->line[0])
		add_history(data->line); 
	return 1;
}
