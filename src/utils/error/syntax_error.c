#include "minishell.h"

void	print_syntax_error(char *msg)
{
    ft_putstr_fd("minishell: syntax error: ", 2);
    ft_putendl_fd(msg, 2);
}