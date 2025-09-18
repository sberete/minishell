/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:39:43 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 03:31:14 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char *name)
{
	if (str_eq(name, "echo"))
		return (1);
	if (str_eq(name, "cd"))
		return (1);
	if (str_eq(name, "pwd"))
		return (1);
	if (str_eq(name, "export"))
		return (1);
	if (str_eq(name, "unset"))
		return (1);
	if (str_eq(name, "env"))
		return (1);
	if (str_eq(name, "exit"))
		return (1);
	return (0);
}

int	run_builtin_argv(t_data *data, char **av)
{
	if (!av || !av[0])
		return (0);
	if (str_eq(av[0], "echo"))
		return (builtin_echo(data, av));
	if (str_eq(av[0], "cd"))
		return (builtin_cd(data, av));
	if (str_eq(av[0], "pwd"))
		return (builtin_pwd(data, av));
	if (str_eq(av[0], "export"))
		return (builtin_export(data, av));
	if (str_eq(av[0], "unset"))
		return (builtin_unset(data, av));
	if (str_eq(av[0], "env"))
		return (builtin_env(data, av));
	if (str_eq(av[0], "exit"))
		return (builtin_exit(data, av));
	return (1);
}

int	exec_builtin(t_data *data, t_ast *cmd)
{
	char	**av;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	av = cmd->argv;
	if (str_eq(av[0], "echo"))
		return (builtin_echo(data, av));
	if (str_eq(av[0], "cd"))
		return (builtin_cd(data, av));
	if (str_eq(av[0], "pwd"))
		return (builtin_pwd(data, av));
	if (str_eq(av[0], "export"))
		return (builtin_export(data, av));
	if (str_eq(av[0], "unset"))
		return (builtin_unset(data, av));
	if (str_eq(av[0], "env"))
		return (builtin_env(data, av));
	if (str_eq(av[0], "exit"))
		return (builtin_exit(data, av));
	return (1);
}
