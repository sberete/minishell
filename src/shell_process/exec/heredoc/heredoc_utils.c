/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 20:05:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:45:51 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <string.h>

/* Retire une seule paire de quotes externes '...' ou "..." si présentes */
static size_t rtrim_len(const char *s)
{
	size_t len;

	if (!s)
		return 0;
	len = ft_strlen(s);
	while (len > 0 && (s[len - 1] == '\r' || s[len - 1] == '\n'))
		len--;
	return len;
}

char *hd_strip_outer_quotes(const char *s)
{
	size_t len;

	if (!s)
		return NULL;
	len = rtrim_len(s);
	if (len >= 2 && (s[0] == '\'' || s[0] == '\"') && s[len - 1] == s[0])
		return ft_substr(s, 1, len - 2);
	return ft_substr(s, 0, len);
}

/* Hook d’expansion du contenu du heredoc (à implémenter plus tard).
   Pour l’instant, on renvoie une copie inchangée. */
char *heredoc_expand_line(const char *line, t_data *data)
{
	(void)data;
	if (!line)
		return NULL;
	return ft_strdup(line);
}
