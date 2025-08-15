#include "minishell.h"

char *extract_token(t_data *data, int *i)
{
	char *token = NULL;
	char *part = NULL;

	skip_spaces(data, i);
	if (!data->line[*i])
		return NULL;
	part = extract_operator(data, i);
	if (part)
		return part;
	while (data->line[*i] && data->line[*i] != ' ' && !is_operator_start(data->line[*i]))
	{
		if (data->line[*i] == '\'' || data->line[*i] == '"')
			part = extract_quoted(data, i);
		else
			part = extract_simple(data, i);
		if (!part)
			return free(token), NULL;
        token = append_token_part(token, part);
	}
	return token;
}

int	tokenize_line(t_data *data)
{
	int		i;
	char	*value;
	t_token	*new;

	i = 0;
	while (data->line[i])
	{
		value = extract_token(data, &i);
		if (!value || *value == '\0')
		{
			free(value);
			continue ;
		}
		new = new_token_node(value);
		if (!new || !add_token_back(&data->tokens, new))
		{
			free(value);
			return (1);
		}
	}
	return (0);
}
