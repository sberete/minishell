#include "minishell.h"

void	builtin_err(char *cmd, char *msg)
{
	char	*c;
	char	*m;

	c = cmd;
	if (!c)
		c = "(null)";
	m = msg;
	if (!m)
		m = "";
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)c, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd((char *)m, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

void	builtin_err_arg(char *cmd, char *arg, char *msg)
{
	char	*c;
	char	*a;
	char	*m;

	c = cmd;
	if (!c)
		c = "(null)";
	a = arg;
	if (!a)
		a = "(null)";
	m = msg;
	if (!m)
		m = "";
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(c, STDERR_FILENO);
	ft_putstr_fd(": `", STDERR_FILENO);
	ft_putstr_fd(a, STDERR_FILENO);
	ft_putstr_fd("': ", STDERR_FILENO);
	ft_putstr_fd(m, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}
