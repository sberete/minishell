/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var_core.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:09:19 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:35:29 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	append_slice(char **dst, char *src, size_t off, size_t len)
{
	char	*part;
	char	*join;

	if (len == 0)
		return (0);
	part = ft_substr(src, off, len);
	if (!part)
		return (1);
	if (!*dst)
		*dst = part;
	else
	{
		join = ft_strjoin(*dst, part);
		free(part);
		if (!join)
			return (1);
		free(*dst);
		*dst = join;
	}
	return (0);
}

int	append_cstr(char **dst, char *s)
{
	char	*join;

	if (!s)
		return (0);
	if (!*dst)
	{
		*dst = ft_strdup(s);
		return (*dst == NULL);
	}
	join = ft_strjoin(*dst, s);
	if (!join)
		return (1);
	free(*dst);
	*dst = join;
	return (0);
}
