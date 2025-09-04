#include "minishell.h"

void    print_env(t_env *lst)
{
    while (lst)
    {
        if (lst->value && *lst->value) // affiche seulement les var avec une valeur
            printf("%s=%s\n", lst->key, lst->value);
        lst = lst->next;
    }
}
