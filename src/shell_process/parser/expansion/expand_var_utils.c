/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:09:31 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:24:18 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* début de nom de var: [A-Za-z_] */
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

/* extrait le nom de variable à partir de s[i] (i pointe APRES le '$')
   - '?' -> name="?" et renvoie l’index suivant
   - identifiant classique -> name="FOO" et renvoie l’index après le nom
   - sinon -> name="" et renvoie i (rien à consommer) */
size_t	var_name_extract(char *s, size_t i, char **name_out)
{
	size_t	j;

	if (!s || !name_out)
		return (i);
	*name_out = NULL;
	if (s[i] == '?')
	{
		*name_out = ft_strdup("?");
		return (i + 1);
	}
	if (!var_name_start(s[i]))
	{
		*name_out = ft_strdup("");
		return (i);
	}
	j = i + 1;
	while (s[j] && var_name_char(s[j]))
		j++;
	*name_out = ft_substr(s, i, j - i);
	return (j);
}

/* valeur expandée pour un nom:
   - "?" -> itoa(last_exit)
   - sinon -> valeur d'env ("" si absente) */
char	*var_expand_value(char *name, t_data *data)
{
	char	*val;

	if (!name || !data)
		return (NULL);
	if (name[0] == '?' && name[1] == '\0')
		return (ft_itoa(data->last_exit));
	val = env_get(data->env, (char *)name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}
