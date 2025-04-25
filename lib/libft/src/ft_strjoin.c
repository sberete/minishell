/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 18:07:38 by sberete           #+#    #+#             */
/*   Updated: 2025/04/03 17:17:37 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	if (!s1 || !s2)
		return (NULL);
	if (s1[ft_strlen(s1) - 1] != '/')
		j = ft_strlen(s1) + 1 + ft_strlen(s2);
	else
		j = ft_strlen(s1) + ft_strlen(s2);
	str = malloc(sizeof(char) * (j + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (*s1)
		str[i++] = *s1++;
	if (s1[-1] != '/')
		str[i++] = '/';
	while (*s2)
		str[i++] = *s2++;
	str[i] = '\0';
	return (str);
}
/*
int	main(void)
{
	char	s1[] = "Je suis venu";
	char	s2[] = " a l'ecole";
	char	*x;

	x = ft_strjoin(s1, s2);
	printf(""%s", x);
}
*/