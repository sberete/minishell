/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/08/14 22:20:18 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_operator_start(char c)
{
	return (c == '|' || c == '&' || c == '<' || c == '>' || c == ';' || c == '('
		|| c == ')' || c == '*');
}

static int	get_operator_len(const char *str)
{
	if (!str)
		return (0);
	if (str[0] && str[1])
	{
		if (strncmp(str, "&&", 2) == 0 || strncmp(str, "||", 2) == 0
			|| strncmp(str, ">>", 2) == 0 || strncmp(str, "<<", 2) == 0)
			return (2);
	}
	if (is_operator_start(str[0]))
		return (1);
	return (0);
}

char	*extract_token(const char *line, int *i)
{
	char	*token;
	char	*part;
	char	quote;
	int		start;
	int		op_len;
	char	*tmp;

	token = NULL;
	part = NULL;
	while (line[*i] == ' ')
		(*i)++;
	if (!line[*i])
		return (NULL);
	op_len = get_operator_len(&line[*i]);
	if (op_len > 0)
	{
		start = *i;
		*i += op_len;
		return (ft_substr(line, start, op_len));
	}
	while (line[*i] && line[*i] != ' ' && !is_operator_start(line[*i]))
	{
		if (line[*i] == '\'' || line[*i] == '"')
		{
			quote = line[*i];
			start = ++(*i);
			while (line[*i] && line[*i] != quote)
				(*i)++;
			if (!line[*i])
			{
				fprintf(stderr, "Erreur: quote non fermée\n");
				free(token);
				return (NULL);
			}
			part = ft_substr(line, start, *i - start);
			(*i)++;
		}
		else
		{
			start = *i;
			while (line[*i] && line[*i] != ' ' && !is_operator_start(line[*i])
				&& line[*i] != '\'' && line[*i] != '"')
				(*i)++;
			part = ft_substr(line, start, *i - start);
		}
		if (token)
		{
			tmp = token;
			token = ft_strjoin(token, part);
			free(tmp);
		}
		else
			token = ft_strdup(part);
		free(part);
	}
	return (token);
}

int	tokenize_line(const char *line, t_token **tokens)
{
	int		i;
	char	*value;
	t_token	*new;

	i = 0;
	while (line[i])
	{
		if (line[i] == ' ')
		{
			i++;
			continue ;
		}
		value = extract_token(line, &i);
		if (!value || *value == '\0')
		{
			free(value);
			continue ;
		}
		new = new_token_node(value);
		if (!new || !add_token_back(tokens, new))
		{
			free(value);
			return (1);
		}
	}
	return (0);
}
void	print_list(t_token *tokens)
{
	t_token	*tmp;

	printf("Tokens extraits :\n");
	tmp = tokens;
	while (tmp)
	{
		printf("Token: %-10s | Type: %d\n", tmp->value, tmp->type);
		tmp = tmp->next;
	}
}
int	main(void)
{
	t_token	*tokens;
	t_data data;

	memset(&data, 0, sizeof(t_data));
	while (1)
	{
		data.line = readline("minishell > ");
		if (!data.line)
		{
			printf("exit\n");
			break ;
		}
		if (*data.line)
			add_history(data.line);
		tokens = NULL;
		if (tokenize_line(data.line, &tokens) != 0)
		{
			printf("Erreur de tokenisation\n");
			free_token_list(tokens);
			free(data.line);
			continue ;
		}
		print_list(tokens);
		free_token_list(tokens);
		free(data.line);
	}
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/

/*
Tokenization
Parcer -> AST
*/