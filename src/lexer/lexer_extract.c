#include "minishell.h"

char	*extract_operator(t_data *data, int *i)
{
	int		op_len;
	char	*token;

	op_len = get_operator_len(&data->line[*i]);
	if (op_len > 0)
	{
		token = ft_substr(data->line, *i, op_len);
		if (!token)
			return (NULL);
		*i += op_len;
		return (token);
	}
	return (NULL);
}

char	*extract_simple(t_data *data, int *i)
{
	int	start;

	start = *i;
	while (data->line[*i]
		&& data->line[*i] != ' '
		&& !is_operator_start(data->line[*i])
		&& data->line[*i] != '\''
		&& data->line[*i] != '"')
		(*i)++;
	return (ft_substr(data->line, start, *i - start));
}

char	*extract_quoted(t_data *data, int *i)
{
	char	quote;
	int		start;
	char	*token;

	quote = data->line[*i];
	start = ++(*i); // skip quote opening
	while (data->line[*i] && data->line[*i] != quote)
		(*i)++;
	if (!data->line[*i])
	{
		print_syntax_error("unslosed quote");
		return (NULL);
	}
	token = ft_substr(data->line, start, *i - start);
	if (!token)
		return (NULL);
	(*i)++; // skip quote closing
	return (token);
}

void	skip_spaces(t_data *data, int *i)
{
	while (data->line[*i] == ' ')
		(*i)++;
}

char *append_token_part(char *token, char *part)
{
    char *tmp;

    if (!part)
        return token;
    if (token)
    {
        tmp = token;
        token = ft_strjoin(token, part);
        free(tmp);
    }
    else
        token = ft_strdup(part);
    free(part);
    return token;
}


