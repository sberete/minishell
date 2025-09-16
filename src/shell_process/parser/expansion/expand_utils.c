/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:51:08 by sberete           #+#    #+#             */
/*   Updated: 2025/09/16 12:47:34 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* get value (non dupliquée) */
char	*env_get(t_env *lst, char *key)
{
	while (lst)
	{
		if (ft_strcmp(lst->key, key) == 0)
			return (lst->value);
		lst = lst->next;
	}
	return (NULL);
}

/* met à jour l'état des quotes */
void	quote_step(char c, int *in_s, int *in_d)
{
	if (!in_s || !in_d)
		return ;
	if (*in_d == 0 && c == '\'')
		*in_s = (*in_s == 0);
	else if (*in_s == 0 && c == '\"')
		*in_d = (*in_d == 0);
}

/* concat dst + src, libère dst, retourne le nouveau (ou NULL) */
char	*str_append_free(char *dst, const char *src)
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
char	*substr_dup(const char *s, size_t pos, size_t len)
{
	char	*out;

	out = (char *)malloc(len + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s + pos, len);
	out[len] = '\0';
	return (out);
}
#include "minishell.h"

/* --- utils : retirer toutes les quotes ' " du lexème (sans interpréter \) --- */
static char *unquote_all(const char *s)
{
	size_t i, j, len;
	char in_s, in_d;
	char *out;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	out = (char *)malloc(len + 1);
	if (!out)
		return (NULL);
	in_s = 0;
	in_d = 0;
	i = 0;
	j = 0;
	while (i < len)
	{
		if (s[i] == '\'' && !in_d)
		{
			in_s = !in_s;
			i++;
			continue ;
		}
		if (s[i] == '\"' && !in_s)
		{
			in_d = !in_d;
			i++;
			continue ;
		}
		out[j++] = s[i++];
	}
	out[j] = '\0';
	return (out);
}

/* argv: d’abord expansion de variables (si can_expand), puis retrait des quotes */
char **expand_argv_vars(char **argv, bool *argv_can_expand, t_data *data)
{
	size_t i, n;
	char **out, *tmp, *uq;

	/* compter */
	n = 0;
	while (argv && argv[n])
		n++;
	out = (char **)malloc((n + 1) * sizeof(char *));
	if (!out)
		return (NULL);
	i = 0;
	while (i < n)
	{
		tmp = NULL;
		if (!argv_can_expand || argv_can_expand[i])
			tmp = ms_expand_vars(argv[i], data);
		else
			tmp = ft_strdup(argv[i]);
		if (!tmp)
			return (free_tab(out), (char **)NULL);
		uq = unquote_all(tmp);
		free(tmp);
		if (!uq)
			return (free_tab(out), (char **)NULL);
		out[i] = uq;
		i++;
	}
	out[n] = NULL;
	return (out);
}

/* redirections: expand filename in-place si autorisé, puis retirer quotes */
int expand_redirs_inplace(t_redir *r, t_data *data)
{
	char *tmp, *uq;

	while (r)
	{
		if (r->filename && r->filename_can_expand)
		{
			tmp = ms_expand_vars(r->filename, data);
			if (!tmp)
				return (1);
			uq = unquote_all(tmp);
			free(tmp);
			if (!uq)
				return (1);
			free(r->filename);
			r->filename = uq;
		}
		/* le heredoc est géré à l’entrée (child heredoc), on ne touche pas r->delim ici */
		r = r->next;
	}
	return (0);
}

char **expand_argv_full(t_ast *cmd, t_data *data)
{
	char **vars;
	char **final;
	size_t n;

	if (!cmd)
		return (NULL);
	/* argv vide (commande avec seulement des redirs) -> tableau vide */
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
