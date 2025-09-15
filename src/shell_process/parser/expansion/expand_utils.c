/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:51:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 15:10:50 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* get value (non dupliquée) */
char	*env_get(t_env *lst, char *key)
{
	while (lst)
	{
		if (ft_strcmp(lst->key, key) == 0)
			return (lst->value);
		lst = lst->next;
	}
	return (NULL);
}

/* met à jour l'état des quotes */
void	quote_step(char c, int *in_s, int *in_d)
{
	if (!in_s || !in_d)
		return ;
	if (*in_d == 0 && c == '\'')
		*in_s = (*in_s == 0);
	else if (*in_s == 0 && c == '\"')
		*in_d = (*in_d == 0);
}

/* concat dst + src, libère dst, retourne le nouveau (ou NULL) */
char	*str_append_free(char *dst, const char *src)
{
	char	*res;

	if (!src)
		return (dst);
	if (!dst)
		return (ft_strdup(src));
	res = ft_strjoin(dst, src);
	if (dst)
		free(dst);
	return (res);
}

/* duplique une tranche s[pos..pos+len[, retourne NULL si fail */
char	*substr_dup(const char *s, size_t pos, size_t len)
{
	char	*out;

	out = (char *)malloc(len + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s + pos, len);
	out[len] = '\0';
	return (out);
}
