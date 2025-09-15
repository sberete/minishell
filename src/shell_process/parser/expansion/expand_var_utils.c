#include "minishell.h"

/* début de nom de var: [A-Za-z_] ou '?' (géré à part côté appelant) */
int	var_name_start(char c)
{
	if (ft_isalpha((unsigned char)c))
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

/* suite de nom: [A-Za-z0-9_] */
int	var_name_char(char c)
{
	if (ft_isalnum((unsigned char)c))
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

/* calcule la longueur du nom à partir de s[pos] (après $) */
size_t	var_name_len(const char *s, size_t pos)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	if (!var_name_start(s[pos]))
		return (0);
	while (s[pos + i] && var_name_char(s[pos + i]))
		i++;
	return (i);
}

/* valeur d'une variable: $? -> last_exit, sinon env ; dup retournée */
char	*var_value_dup(const char *name, t_data *data)
{
	char	*val;
	char	*dup;

	if (!name || !data)
		return (NULL);
	if (name[0] == '?' && name[1] == '\0')
	{
		dup = ft_itoa(data->last_exit);
		return (dup);
	}
	val = env_get(data->env, (char *)name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}
