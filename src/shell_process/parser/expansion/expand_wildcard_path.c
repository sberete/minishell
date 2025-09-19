/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard_path.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:46:47 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:46:52 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	want_hidden_first(char *basepat)
{
	if (basepat && basepat[0] == '.')
		return (1);
	return (0);
}

char	*join_dir_name(char *dir, char *name)
{
	if (!dir || dir[0] == '\0')
		return (ft_join_sep(".", "/", name));
	if (dir[0] == '/' && dir[1] == '\0')
		return (ft_join_sep("/", "", name));
	return (ft_join_sep(dir, "/", name));
}

char	*dup_dirpath(char *pat, char **base)
{
	char	*slash;

	*base = pat;
	if (!pat)
		return (ft_strdup("."));
	slash = ft_strrchr(pat, '/');
	if (!slash)
		return (ft_strdup("."));
	*base = slash + 1;
	if (slash == pat)
		return (ft_strdup("/"));
	return (substr_dup(pat, 0, (size_t)(slash - pat)));
}
