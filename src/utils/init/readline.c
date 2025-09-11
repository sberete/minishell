#include "minishell.h"

int read_input(t_data *data)
{
    char   *line;
    size_t  i;

    line = readline("minishell > ");
    data->line = line;

    if (line == NULL)
    {
        printf("exit\n");
        rl_clear_history();
        return 0;                    /* EOF (Ctrl-D) */
    }
    /* skip lignes blanches (dont celles dues à Ctrl-C) */
    i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;
    if (line[i] == '\0')
    {
        free(line);
        data->line = NULL;
        return 2;                    /* vide: ignorer ce tour */
    }
    add_history(line);
    return 1;
}
