/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_argv_vars.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:12:53 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 02:20:43 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_one(char *arg, int can_exp, t_data *d)
{
	char	*tmp;
	char	*out;

	if (can_exp)
		tmp = ms_expand_vars(arg, d);
	else
		tmp = ft_strdup(arg);
	if (!tmp)
		return (NULL);
	out = unquote_all(tmp);
	free(tmp);
	return (out);
}

char	**expand_argv_vars(char **argv, bool *flags, t_data *data)
{
	size_t	i;
	size_t	n;
	char	**out;
	int		can_exp;

	n = ft_tablen(argv);
	out = (char **)malloc(sizeof(char *) * (n + 1));
	if (!out)
		return (NULL);
	i = 0;
	while (i < n)
	{
		can_exp = 1;
		if (flags && flags[i] == false)
			can_exp = 0;
		out[i] = expand_one(argv[i], can_exp, data);
		if (!out[i])
		{
			free_tab(out);
			return (NULL);
		}
		i++;
	}
	out[n] = NULL;
	return (out);
}
