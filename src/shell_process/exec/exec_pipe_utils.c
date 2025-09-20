/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 22:40:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:44:46 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	xclose(int *pfd)
{
	if (*pfd >= 0)
	{
		close(*pfd);
		*pfd = -1;
	}
}

int	dup2_and_close(int *src, int dst, char *ctx)
{
	if (src < 0)
		return (0);
	if (dup2(*src, dst) < 0)
		return (err_sys_ctx(ctx));
	xclose(src);
	return (0);
}

pid_t	spawn_left(t_ast *sub, t_data *data, int pipe[2])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (-1 * err_sys_ctx("fork"));
	if (pid == 0)
	{
		xclose(&pipe[0]);
		if (dup2_and_close(&pipe[1], STDOUT_FILENO, "dup2 stdout"))
		{
			exec_ast(sub, data);
			free_ast(data->ast);
			free_env(&data->env);
			exit(1);
		}
		xclose(&pipe[0]);
		exec_ast(sub, data);
		free_ast(data->ast);
		free_env(&data->env);
		exit(0);
	}
	return (pid);
}

pid_t	spawn_right(t_ast *sub, t_data *data, int pipe[2])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (-1 * err_sys_ctx("fork"));
	if (pid == 0)
	{
		if (dup2_and_close(&pipe[0], STDIN_FILENO, "dup2 stdin"))
		{
			exec_ast(sub, data);
			free_ast(data->ast);
			free_env(&data->env);
			exit(1);
		}
		xclose(&pipe[0]);
		exec_ast(sub, data);
		free_ast(data->ast);
		free_env(&data->env);
		exit(0);
	}
	return (pid);
}
