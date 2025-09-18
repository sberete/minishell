/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_join_sep.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 01:14:37 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 01:14:38 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	s_len(const char *s)
{
	if (!s)
		return (0);
	return (ft_strlen(s));
}

static void	s_cat(char *dst, size_t *i, const char *src)
{
	size_t	len;

	if (!src)
		return ;
	len = ft_strlen(src);
	ft_memcpy(dst + *i, src, len);
	*i += len;
}

char	*ft_join_sep(const char *a, const char *b, const char *c)
{
	char	*s;
	size_t	la;
	size_t	lb;
	size_t	lc;
	size_t	i;

	la = s_len(a);
	lb = s_len(b);
	lc = s_len(c);
	s = malloc(la + lb + lc + 1);
	if (!s)
		return (NULL);
	i = 0;
	s_cat(s, &i, a);
	s_cat(s, &i, b);
	s_cat(s, &i, c);
	s[i] = '\0';
	return (s);
}
