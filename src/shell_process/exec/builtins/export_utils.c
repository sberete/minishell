/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 04:56:32 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 04:56:36 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	id_is_valid(char *s)
{
	size_t	i;

	if (!s || !*s)
		return (0);
	if (!(ft_isalpha(s[0]) || s[0] == '_'))
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

static int	env_append_local(t_env **lst, char *key, char *suffix)
{
	t_env	*cur;
	char	*joined;

	if (!lst || !key || !suffix)
		return (-1);
	cur = env_find(*lst, key);
	if (!cur)
		return (env_set(lst, key, suffix, 1));
	if (cur->value)
		joined = ft_strjoin(cur->value, suffix);
	else
		joined = ft_strdup(suffix);
	if (!joined)
		return (-1);
	free(cur->value);
	cur->value = joined;
	return (0);
}

int	export_set_assignment(t_env **lst, char *assign)
{
	char	*key;
	char	*val;
	int		append;
	int		rc;

	if (parse_assignment(assign, &key, &val, &append) != 0)
		return (1);
	if (append)
		rc = env_append_local(lst, key, val);
	else
		rc = env_set(lst, key, val, 1);
	free(key);
	free(val);
	if (rc != 0)
		return (1);
	return (0);
}
