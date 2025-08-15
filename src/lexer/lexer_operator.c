#include "minishell.h"

int	is_operator_start(char c)
{
	return (c == '|' || c == '&' || c == '<' || c == '>' || c == ';' || c == '('
		|| c == ')' || c == '*');
}

int	get_operator_len(const char *str)
{
	if (!str)
		return (0);
	if (str[0] && str[1])
	{
		if (ft_strncmp(str, "&&", 2) == 0 || ft_strncmp(str, "||", 2) == 0
			|| ft_strncmp(str, ">>", 2) == 0 || ft_strncmp(str, "<<", 2) == 0)
			return (2);
	}
	if (is_operator_start(str[0]))
		return (1);
	return (0);
}
