/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_quote.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:08:20 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 02:16:48 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	uq_state_init(size_t *i, size_t *j, int *in_s, int *in_d)
{
	*i = 0;
	*j = 0;
	*in_s = 0;
	*in_d = 0;
}

static int	uq_toggle_if_quote(char c, int *in_s, int *in_d)
{
	if (c == '\'' && *in_d == 0)
	{
		if (*in_s == 0)
			*in_s = 1;
		else
			*in_s = 0;
		return (1);
	}
	if (c == '\"' && *in_s == 0)
	{
		if (*in_d == 0)
			*in_d = 1;
		else
			*in_d = 0;
		return (1);
	}
	return (0);
}

/* conservée car utilisée ailleurs */
void	quote_step(char c, int *in_s, int *in_d)
{
	if (!in_s || !in_d)
		return ;
	(void)uq_toggle_if_quote(c, in_s, in_d);
}

char	*unquote_all(char *s)
{
	char	*out;
	size_t	i;
	size_t	j;
	int		in_s;
	int		in_d;

	if (!s)
		return (NULL);
	out = (char *)malloc(ft_strlen(s) + 1);
	if (!out)
		return (NULL);
	uq_state_init(&i, &j, &in_s, &in_d);
	while (s[i])
	{
		if (!uq_toggle_if_quote(s[i], &in_s, &in_d))
		{
			out[j] = s[i];
			j++;
		}
		i++;
	}
	out[j] = '\0';
	return (out);
}
