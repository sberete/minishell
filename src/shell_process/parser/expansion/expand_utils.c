/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:51:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 02:13:58 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* concat dst + src, libère dst, retourne le nouveau (ou NULL) */
char	*str_append_free(char *dst, char *src)
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
char	*substr_dup(char *s, size_t pos, size_t len)
{
	char	*out;

	out = (char *)malloc(len + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s + pos, len);
	out[len] = '\0';
	return (out);
}

char	**expand_argv_full(t_ast *cmd, t_data *data)
{
	char	**vars;
	char	**final;
	size_t	n;

	if (!cmd)
		return (NULL);
	n = 0;
	while (cmd->argv && cmd->argv[n])
		n++;
	if (n == 0)
	{
		final = (char **)malloc(sizeof(char *));
		if (!final)
			return (NULL);
		final[0] = NULL;
		return (final);
	}
	vars = expand_argv_vars(cmd->argv, cmd->argv_can_expand, data);
	if (!vars)
		return (NULL);
	final = expand_argv_glob(vars);
	free_tab(vars);
	return (final);
}
