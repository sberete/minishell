#include "minishell.h"

t_token_type	get_token_type(const char *str)
{
	if (!str)
		return (T_UNKNOWN);
	if (strcmp(str, "&&") == 0)
		return (T_AND);
	else if (strcmp(str, "||") == 0)
		return (T_OR);
	else if (strcmp(str, "|") == 0)
		return (T_PIPE);
	else if (strcmp(str, "<<") == 0)
		return (T_HEREDOC);
	else if (strcmp(str, ">>") == 0)
		return (T_APPEND);
	else if (strcmp(str, "<") == 0)
		return (T_REDIR_IN);
	else if (strcmp(str, ">") == 0)
		return (T_REDIR_OUT);
	else if (strcmp(str, ";") == 0)
		return (T_SEPARATOR);
	else if (strcmp(str, "(") == 0)
		return (T_PAREN_OPEN);
	else if (strcmp(str, ")") == 0)
		return (T_PAREN_CLOSE);
	else if (strcmp(str, "*") == 0)
		return (T_WILDCARD);
	else if (*str == '\0')
		return (T_END);
	return (T_WORD);
}

t_token	*new_token_node(char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = value;
	token->type = get_token_type(value);
	token->next = NULL;
	printf("new_token_node: created token [%s] with type %d\n", value,
		token->type);
	return (token);
}

bool	add_token_back(t_token **head, t_token *new)
{
	t_token	*tmp;

	if (!new)
		return (false);
	if (!*head)
	{
		*head = new;
		printf("add_token_back: added token [%s] as head\n", new->value);
		return (true);
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
	printf("add_token_back: added token [%s] at the end\n", new->value);
	return (true);
}

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

char *extract_token(const char *line, int *i)
{
	int		start;
	char	*token;
	char	quote = '\0';

	// Skip leading spaces
	while (line[*i] == ' ')
		(*i)++;

	if (!line[*i])
		return (NULL);

	// Handle operators like &&, ||, etc.
	int op_len = get_operator_len(&line[*i]);
	if (op_len > 0)
	{
		start = *i;
		*i += op_len;
		token = ft_substr(line, start, op_len);
		printf("extract_token: operator token=[%s]\n", token);
		return (token);
	}

	// Handle quoted strings
	if (line[*i] == '\'' || line[*i] == '\"')
	{
		quote = line[*i];
		start = ++(*i); // Skip opening quote
		while (line[*i] && line[*i] != quote)
			(*i)++;
		token = ft_substr(line, start, *i - start); // Extrait sans quotes
		if (line[*i] == quote)
			(*i)++; // Skip closing quote
		printf("extract_token: quoted token=[%s]\n", token);
		return (token);
	}

	// Normal word (non-quoted)
	start = *i;
	while (line[*i] && line[*i] != ' ' && !is_operator_start(line[*i])
		&& line[*i] != '\'' && line[*i] != '\"')
		(*i)++;
	token = ft_substr(line, start, *i - start);
	printf("extract_token: word token=[%s]\n", token);
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

void	free_token_list(t_token *head)
{
	t_token	*tmp;

	while (head)
	{
		tmp = head->next;
		free(head->value);
		free(head);
		head = tmp;
	}
}

int	main(void)
{
	char	*line;
	t_token	*tokens;
	t_token	*tmp;

	while (1)
	{
		line = readline("minishell > ");
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (*line)
			add_history(line);
		tokens = NULL;
		if (tokenize_line(line, &tokens) != 0)
		{
			printf("Erreur de tokenisation\n");
			free_token_list(tokens);
			free(line);
			continue ;
		}
		printf("Tokens extraits :\n");
		tmp = tokens;
		while (tmp)
		{
			printf("Token: %-10s | Type: %d\n", tmp->value, tmp->type);
			tmp = tmp->next;
		}
		free_token_list(tokens);
		free(line);
	}
	return (0);
}

/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/

/*
Tokenization
Parcer -> AST
*/