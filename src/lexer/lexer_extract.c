#include "minishell.h"

char	*extract_operator(t_data *data, int *i)
{
	int op_len = get_operator_len(&data->line[*i]);
	char *token;

	if (op_len > 0)
	{
		token = ft_substr(data->line, *i, op_len);
		*i += op_len;
		return token;
	}
	return NULL;
}

char	*extract_simple(t_data *data, int *i)
{
	int start = *i;

	while (data->line[*i] && data->line[*i] != ' ' &&
		   !is_operator_start(data->line[*i]) &&
		   data->line[*i] != '\'' && data->line[*i] != '"')
		(*i)++;

	return ft_substr(data->line, start, *i - start);
}

char	*extract_quoted(t_data *data, int *i)
{
	char quote = data->line[*i];
	int start;
	char *token;

	start = ++(*i); // sauter la quote d'ouverture
	while (data->line[*i] && data->line[*i] != quote)
		(*i)++;
	if (!data->line[*i])
	{
		fprintf(stderr, "Erreur: quote non fermée\n");
		return NULL;
	}
	token = ft_substr(data->line, start, *i - start);
	(*i)++; // sauter la quote de fermeture
	return token;
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


