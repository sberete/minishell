/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_parse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 04:59:58 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 05:00:03 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ---------- aides courtes ---------- */

static int	compute_klen_and_append(char *s, char *eq,
					int *append, size_t *klen)
{
	*append = 0;
	if (eq > s && *(eq - 1) == '+')
	{
		*append = 1;
		*klen = (size_t)(eq - s - 1);
	}
	else
		*klen = (size_t)(eq - s);
	return (0);
}

static int	alloc_key_slice_check_id(char *s, size_t klen, char **key)
{
	*key = (char *)malloc(klen + 1);
	if (!*key)
		return (-1);
	ft_memcpy(*key, s, klen);
	(*key)[klen] = '\0';
	if (!id_is_valid(*key))
	{
		free(*key);
		*key = NULL;
		return (-1);
	}
	return (0);
}

/* ---------- parseurs ---------- */

static int	parse_no_equal(char *s, char **key, char **val)
{
	if (!id_is_valid(s))
		return (-1);
	*key = ft_strdup(s);
	if (!*key)
		return (-1);
	*val = ft_strdup("");
	if (!*val)
	{
		free(*key);
		*key = NULL;
		return (-1);
	}
	return (0);
}

static int	parse_equal(char *s, char **key, char **val, int *append)
{
	char	*eq;
	size_t	klen;

	eq = ft_strchr(s, '=');
	if (!eq)
		return (-1);
	if (compute_klen_and_append(s, eq, append, &klen) != 0)
		return (-1);
	if (alloc_key_slice_check_id(s, klen, key) != 0)
		return (-1);
	*val = ft_strdup(eq + 1);
	if (!*val)
	{
		free(*key);
		*key = NULL;
		return (-1);
	}
	return (0);
}

/* PUBLIC: KEY / KEY=VAL / KEY+=VAL ; 0 si OK, -1 sinon */
int	parse_assignment(char *s, char **key, char **val, int *append)
{
	if (!s || !key || !val || !append)
		return (-1);
	*key = NULL;
	*val = NULL;
	*append = 0;
	if (ft_strchr(s, '=') == NULL)
		return (parse_no_equal(s, key, val));
	return (parse_equal(s, key, val, append));
}
