#include "minishell.h"

int	id_is_valid(const char *s)
{
	size_t	i;

	if (!s || !*s)
		return (0);
	if (!(ft_isalpha(*s) || *s == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

int	split_key_value(const char *s, char **k, char **v)
{
	char	*eq;

	*k = NULL;
	*v = NULL;
	if (!s)
		return (-1);
	eq = ft_strchr(s, '=');
	if (!eq)
		return (-1); // pas d' '='
	*k = ft_substr(s, 0, eq - s);
	*v = ft_strdup(eq + 1);
	if (!*k || !*v)
	{
		free(*k);
		free(*v);
		*k = NULL;
		*v = NULL;
		return (-1);
	}
	return (0);
}

int	split_key_append(const char *s, char **k, char **v, int *is_append)
{
	char	*plus;
	char	*eq;

	*k = NULL;
	*v = NULL;
	*is_append = 0;
	if (!s)
		return (-1);
	eq = ft_strchr(s, '=');
	if (!eq)
		return (-1);
	// Cherche pattern KEY+=
	plus = eq - 1;
	if (plus >= s && *plus == '+')
	{
		*is_append = 1;
		*k = ft_substr(s, 0, plus - s);
	}
	else
		*k = ft_substr(s, 0, eq - s);
	*v = ft_strdup(eq + 1);
	if (!*k || !*v)
	{
		free(*k);
		free(*v);
		*k = NULL;
		*v = NULL;
		*is_append = 0;
		return (-1);
	}
	return (0);
}

char	*ms_getenv(t_env *lst, const char *key)
{
	t_env	*n;

	n = env_find(lst, key);
	return (n ? n->value : NULL);
}

int	ms_setenv(t_env **lst, const char *key, const char *value, int overwrite)
{
	t_env	*n;

	if (!key || !id_is_valid(key))
		return (-1);
	n = env_find(*lst, key);
	if (!n)
	{
		env_add_back(lst, env_new(key, value ? value : ""));
		return (0);
	}
	if (!overwrite)
		return (0);
	free(n->value);
	n->value = ft_strdup(value ? value : "");
	if (!n->value)
	{
		perror("strdup");
		exit(1);
	}
	return 0;
}

int	ms_setenv_eq(t_env **lst, const char *assign)
{
	char	*k;
	char	*v;

	if (split_key_value(assign, &k, &v) < 0)
		return -1;
	if (!id_is_valid(k))
	{
		free(k);
		free(v);
		return -1;
	}
	ms_setenv(lst, k, v, 1);
	free(k);
	free(v);
	return 0;
}

int ms_setenv_append(t_env **lst, const char *assign)
{
    char *k;
    char *v;
    int is_append;
    t_env *n;

    if (split_key_append(assign, &k, &v, &is_append) < 0 || !is_append)
        return -1;
    if (!id_is_valid(k))
    {
        free(k); free(v);
        return -1;
    }
    n = env_find(*lst, k);
    if (!n)
        env_add_back(lst, env_new(k, v));
    else
    {
        char *joined = ft_strjoin(n->value, v);
        if (!joined) { perror("strjoin"); exit(1); }
        free(n->value);
        n->value = joined;
    }
    free(k); free(v);
    return 0;
}
