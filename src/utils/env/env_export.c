#include "minishell.h"

char	**env_to_environ(t_env *lst)
{
	size_t	n;
	char	**tab;
	size_t	i;
	char	*kv;
	size_t	len;

	n = env_size(lst);
	tab = (char **)malloc(sizeof(char *) * (n + 1));
	i = 0;
	if (!tab)
	{
		perror("malloc");
		exit(1);
	}
	while (lst)
	{
		len = ft_strlen(lst->key) + 1 + ft_strlen(lst->value);
		kv = (char *)malloc(len + 1);
		if (!kv)
		{
			perror("malloc");
			exit(1);
		}
		// formate "KEY=VALUE"
		ft_strlcpy(kv, lst->key, len + 1);
		ft_strlcat(kv, "=", len + 1);
		ft_strlcat(kv, lst->value, len + 1);
		tab[i++] = kv;
		lst = lst->next;
	}
	tab[i] = NULL;
	return (tab);
}

// Importer depuis char **environ
static void	push_kv(t_env **lst, const char *kv)
{
	char	*k;
	char	*v;

	k = NULL;
	v = NULL;
	if (split_key_value(kv, &k, &v) == 0 && id_is_valid(k))
		env_add_back(lst, env_new(k, v));
	else if (k) // si key invalide, on ignore
	{
		// ignore invalid entries silently (comportement minimal)
	}
	free(k);
	free(v);
}
void	env_inc_shlvl(t_env **lst)
{
	long	lvl;
	char	*cur;
	char	*new_val;

	cur = ms_getenv(*lst, "SHLVL");
	if (!cur)
	{
		ms_setenv(lst, "SHLVL", "1", 1);
		return ;
	}
	lvl = ft_atoi(cur);
	if (lvl < 0)
		lvl = 0;
	if (lvl > 1000)
		lvl = 0;
	lvl += 1;
	new_val = ft_itoa(lvl);
	if (!new_val)
	{
		perror("ft_itoa");
		exit(1);
	}
	ms_setenv(lst, "SHLVL", new_val, 1);
	free(new_val);
}

t_env	*env_from_environ(char **env)
{
	t_env	*lst;
	size_t	i;

	lst = NULL;
	if (!env)
		return (NULL);
	i = 0;
	while (env[i])
	{
		push_kv(&lst, env[i]);
		i++;
	}
	env_inc_shlvl(&lst);
	return (lst);
}
