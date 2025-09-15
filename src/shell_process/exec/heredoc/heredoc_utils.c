#include "minishell.h"

char	*hd_strip_outer_quotes(char *s)
{
	size_t	len;
	char	q;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	if (len >= 2)
	{
		q = s[0];
		if ((q == '\'' || q == '\"') && s[len - 1] == q)
			return (substr_dup(s, 1, len - 2));
	}
	return (ft_strdup(s));
}

char	*heredoc_expand_line(char *line, t_data *data)
{
	return (ms_expand_vars(line, data));
}