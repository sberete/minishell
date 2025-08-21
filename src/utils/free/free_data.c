#include "minishell.h"

void free_data(t_data *data)
{   
    if (!data)
        return;
    if (data->line)
    {
        free(data->line);
        data->line = NULL;
    }
    if (data->tokens)
        free_token_list(&data->tokens);
    if (data->ast)
        free_ast(data->ast);
}
