#include "minishell.h"

static void *xdup(const char *s)
{
char *p;
if (!s)
return NULL;
p = ft_strdup(s);
if (!p)
{
perror("strdup");
exit(1);
}
return p;
}


t_env *env_new(const char *key, const char *value)
{
t_env *n = (t_env *)malloc(sizeof(t_env));
if (!n)
{
perror("malloc");
exit(1);
}
n->key = xdup(key);
n->value = value ? xdup(value) : xdup("");
n->next = NULL;
return n;
}


void env_add_back(t_env **lst, t_env *node)
{
t_env *cur;


if (!lst || !node)
return;
if (!*lst)
{
*lst = node;
return;
}
cur = *lst;
while (cur->next)
cur = cur->next;
cur->next = node;
}


size_t env_size(t_env *lst)
{
size_t n = 0;
while (lst)
{
++n;
lst = lst->next;
}
return n;
}

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