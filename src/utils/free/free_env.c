#include "minishell.h"

static t_env *env_find_prev(t_env *lst, const char *key)
{
t_env *prev = NULL;
while (lst)
{
if (ft_strcmp(lst->key, key) == 0)
return prev; // prev == NULL si premier élément
prev = lst;
lst = lst->next;
}
return NULL; // non trouvé
}


t_env *env_find(t_env *lst, const char *key)
{
while (lst)
{
if (ft_strcmp(lst->key, key) == 0)
return lst;
lst = lst->next;
}
return NULL;
}


int env_remove(t_env **lst, const char *key)
{
t_env *prev;
t_env *cur;


if (!lst || !*lst || !key)
return -1;
if (ft_strcmp((*lst)->key, key) == 0)
{
cur = *lst;
*lst = cur->next;
free(cur->key);
free(cur->value);
free(cur);
return 0;
}
prev = env_find_prev(*lst, key);
if (!prev || !prev->next)
return -1;
cur = prev->next;
prev->next = cur->next;
free(cur->key);
free(cur->value);
free(cur);
return 0;
}