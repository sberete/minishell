#include "minishell.h"

void free_token_list(t_token **head)
{
    t_token *tmp;

    while (*head)
    {
        tmp = (*head)->next;
        free((*head)->value);
        free(*head);
        *head = tmp;
    }
    *head = NULL;
}