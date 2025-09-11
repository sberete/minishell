/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 19:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:24:51 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int is_numeric_ll(const char *s, long long *out)
{
	char       *end;
	long long   v;

	if (!s || *s == '\0')
		return 0;
	errno = 0;
	v = strtoll(s, &end, 10);
	if (errno != 0)
		return 0;
	if (*end != '\0')
		return 0;
	if (out)
		*out = v;
	return 1;
}

static void shell_cleanup_and_exit(t_data *data, int status)
{
	rl_clear_history();
	free_data(data);
	exit(status);
}

/* Comportement proche bash:
   - exit -> imprime "exit" et quitte avec last_exit
   - exit N -> si N non numérique => message, exit 255 ; sinon exit(N % 256)
   - exit N M -> "too many arguments", return 1 (ne quitte pas)
*/
int ft_exit(char **argv, t_data *data)
{
	long long v;
	int       code;
	int       argc;

	ft_putstr_fd("exit\n", STDOUT_FILENO);

	argc = 0;
	while (argv[argc])
		argc++;

	if (argc == 1)
	{
		code = data->last_exit;
		shell_cleanup_and_exit(data, code);
		return code; /* unreachable */
	}
	if (!is_numeric_ll(argv[1], &v))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(argv[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		shell_cleanup_and_exit(data, 255);
		return 255; /* unreachable */
	}
	if (argc > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		return 1;
	}
	code = (unsigned char)v;
	shell_cleanup_and_exit(data, code);
	return code; /* unreachable */
}
