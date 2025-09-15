#include "minishell.h"

/* remplace $... dans s en respectant quotes ; retourne dup expandée */
char	*ms_expand_vars(const char *s, t_data *data)
{
	size_t	i;
	size_t	start;
	int		in_s;
	int		in_d;
	char	*out;
	char	*part;
	char	*name;
	char	*val;
	size_t	nlen;

	if (!s)
		return (NULL);
	i = 0;
	start = 0;
	in_s = 0;
	in_d = 0;
	out = NULL;
	while (s[i])
	{
		quote_step(s[i], &in_s, &in_d);
		if (s[i] == '$' && in_s == 0)
		{
			if (i > start)
			{
				part = substr_dup(s, start, i - start);
				if (!part)
					return (free(out), NULL);
				out = str_append_free(out, part);
				free(part);
			}
			i++;
			if (s[i] == '?')
				nlen = 1;
			else
				nlen = var_name_len(s, i);
			if (nlen == 0)
			{
				out = str_append_free(out, "$");
				start = i;
				continue ;
			}
			name = substr_dup(s, i, nlen);
			if (!name)
				return (free(out), NULL);
			val = var_value_dup(name, data);
			free(name);
			if (!val)
				return (free(out), NULL);
			out = str_append_free(out, val);
			free(val);
			i = i + nlen;
			start = i;
			continue ;
		}
		i++;
	}
	if (i > start)
	{
		part = substr_dup(s, start, i - start);
		if (!part)
			return (free(out), NULL);
		out = str_append_free(out, part);
		free(part);
	}
	return (out);
}

/* applique ms_expand_vars sur argv[i] si can_expand[i]==true ; dup */
char	**expand_argv_vars(char **argv, bool *can_expand, t_data *data)
{
	size_t	n;
	size_t	i;
	char	**out;
	char	*tmp;

	if (!argv)
		return (NULL);
	n = 0;
	while (argv[n])
		n++;
	out = (char **)malloc(sizeof(char *) * (n + 1));
	if (!out)
		return (NULL);
	i = 0;
	while (i < n)
	{
		if (!can_expand || can_expand[i])
			tmp = ms_expand_vars(argv[i], data);
		else
			tmp = NULL;
		if (!tmp)
			out[i] = ft_strdup(argv[i]);
		else
			out[i] = tmp;
		if (!out[i])
		{
			while (i > 0)
			{
				i--;
				free(out[i]);
			}
			free(out);
			return (NULL);
		}
		i++;
	}
	out[n] = NULL;
	return (out);
}
#include "minishell.h"

/* Expand in-place filename/delim selon les flags *_can_expand */
int expand_redirs_inplace(t_redir *r, t_data *data)
{
    for (; r; r = r->next)
    {
        if (r->type == REDIR_HEREDOC)
        {
            if (r->delim && r->delim_can_expand)
            {
                char *e = ms_expand_vars(r->delim, data);
                if (!e) return -1;
                free(r->delim);
                r->delim = e;
            }
        }
        else
        {
            if (r->filename && r->filename_can_expand)
            {
                char *e = ms_expand_vars(r->filename, data);
                if (!e) return -1;
                free(r->filename);
                r->filename = e;
            }
        }
    }
    return 0;
}
