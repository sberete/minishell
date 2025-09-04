#include "minishell.h"

// N.B.: pour execve, on n'exporte que key=value (clé présente même si valeur vide)
char	**env_to_environ(t_env *lst)
{
	size_t	n;
	char	**tab;
	size_t	i;
	char *kv;
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

void	free_strtab(char **tab)
{
	size_t	i;

	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
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

t_env	*env_from_environ(char **environ)
{
	t_env *lst = NULL;
	size_t i;

	if (!environ)
		return (NULL);
	for (i = 0; environ[i]; ++i)
		push_kv(&lst, environ[i]);
	// SHLVL++ pour notre shell
	env_inc_shlvl(&lst);
	return (lst);
}