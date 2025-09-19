/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:22:12 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:23:56 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct s_expv_ctx
{
	char	*s;
	size_t	i;
	size_t	lit_start;
	int		in_s;
	int		in_d;
	char	*out;
	t_data	*data;
}			t_expv_ctx;

static int	flush_lit(t_expv_ctx *c)
{
	return (append_slice(&c->out, (char *)c->s, c->lit_start, c->i
			- c->lit_start));
}

static int	append_and_free(char **out, char *s)
{
	if (!s)
		return (1);
	if (append_cstr(out, s))
	{
		free(s);
		return (1);
	}
	free(s);
	return (0);
}

static int	handle_dollar(t_expv_ctx *c)
{
	size_t	next;
	char	*name;
	char	*val;

	if (!c->s[c->i])
		return (append_cstr(&c->out, "$"));
	next = var_name_extract((char *)c->s, c->i, &name);
	if (!name)
		return (1);
	if (name[0] == '\0' && next == c->i)
	{
		free(name);
		if (append_cstr(&c->out, "$"))
			return (1);
		c->lit_start = c->i;
		return (0);
	}
	val = var_expand_value(name, c->data);
	free(name);
	if (append_and_free(&c->out, val))
		return (1);
	c->i = next;
	c->lit_start = c->i;
	return (0);
}

static int	process_char(t_expv_ctx *c)
{
	if ((c->s[c->i] == '\'' && !c->in_d) || (c->s[c->i] == '\"' && !c->in_s))
	{
		if (flush_lit(c))
			return (1);
		if (c->s[c->i] == '\'')
			c->in_s = !c->in_s;
		else
			c->in_d = !c->in_d;
		c->i++;
		c->lit_start = c->i;
		return (0);
	}
	if (c->s[c->i] == '$' && !c->in_s)
	{
		if (flush_lit(c))
			return (1);
		c->i++;
		if (handle_dollar(c))
			return (1);
		return (0);
	}
	c->i++;
	return (0);
}

/* ---- publique ---- */
char	*ms_expand_vars(char *s, t_data *data)
{
	t_expv_ctx	c;

	if (!s)
		return (NULL);
	c.s = s;
	c.i = 0;
	c.lit_start = 0;
	c.in_s = 0;
	c.in_d = 0;
	c.out = NULL;
	c.data = data;
	while (c.s[c.i])
	{
		if (process_char(&c))
			return (free(c.out), NULL);
	}
	if (flush_lit(&c))
		return (free(c.out), NULL);
	if (c.out)
		return (c.out);
	return (ft_strdup(""));
}
