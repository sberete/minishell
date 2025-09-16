#include "minishell.h"

static int append_slice(char **dst, const char *src, size_t off, size_t len)
{
	char *part, *join;

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

static int append_cstr(char **dst, const char *s)
{
	char *join;

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

char *ms_expand_vars(const char *s, t_data *data)
{
	size_t i, lit_start;
	int    in_s, in_d;
	char  *out, *name, *val;

	if (!s)
		return (NULL);
	in_s = 0;
	in_d = 0;
	out = NULL;
	lit_start = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'' && !in_d) { in_s = !in_s; i++; continue; }
		if (s[i] == '\"' && !in_s) { in_d = !in_d; i++; continue; }
		if (s[i] == '$' && !in_s)
		{
			/* flush le littéral précédent */
			if (append_slice(&out, s, lit_start, i - lit_start))
				return (free(out), NULL);
			i++;
			/* fin de chaîne après '$' -> garder '$' littéral */
			if (!s[i])
			{
				if (append_cstr(&out, "$")) return (free(out), NULL);
				break;
			}
			/* extraire le nom */
			{
				size_t next = var_name_extract(s, i, &name);
				if (!name) return (free(out), NULL);
				if (name[0] == '\0' && next == i)
				{
					/* $ suivi d’un non-identifiant -> littéral '$' */
					free(name);
					if (append_cstr(&out, "$")) return (free(out), NULL);
					lit_start = i; /* on ne consomme rien, on reprendra ici */
					continue;
				}
				/* expand la valeur */
				val = var_expand_value(name, data);
				free(name);
				if (!val) return (free(out), NULL);
				if (append_cstr(&out, val)) { free(val); return (free(out), NULL); }
				free(val);
				i = next;
				lit_start = i;
				continue;
			}
		}
		i++;
	}
	/* flush le reste */
	if (append_slice(&out, s, lit_start, i - lit_start))
		return (free(out), NULL);
	return (out ? out : ft_strdup(""));
}

/* Compat si du code appelle encore l’ancien nom */
char **expand_argv_dup(t_ast *cmd, t_data *data)
{
	return (expand_argv_full(cmd, data));
}
