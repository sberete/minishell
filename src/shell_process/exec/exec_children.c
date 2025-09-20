/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_children.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:05:35 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 18:13:01 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 23:59:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 23:59:00 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	child_prepare_redirs(t_exec *ex, t_ast *cmd)
{
	if (expand_redirs_inplace(ex->redirs, ex->data) != 0)
	{
		err_simple("redirection: expansion failed");
		free_ast(cmd);
		free_data(ex->data);
		exit(1);
	}
	if (apply_redirs(ex->redirs) != 0)
	{
		free_data(ex->data);
		exit(1);
	}
}

static void	child_prepare_argv(t_ast *cmd, t_exec *ex)
{
	ex->argv = expand_argv_dup(cmd, ex->data);
	if (!ex->argv)
	{
		free_ast(cmd);
		free_env(&ex->data->env);
		exit(1);
	}
	if (!ex->argv[0])
	{
		free_ast(cmd);
		free_env(&ex->data->env);
		child_free_and_exit(ex->argv, NULL, NULL, 0);
	}
}

static void	child_try_builtin(t_exec *ex)
{
	int	st;

	if (is_builtin(ex->argv[0]))
	{
		st = run_builtin_argv(ex->data, ex->argv);
		free_data(ex->data);
		free(ex->pid);
		child_free_and_exit(ex->argv, NULL, NULL, st);
	}
}

static void	child_resolve_path(t_exec *ex, t_ast *cmd)
{
	ex->path = find_cmd_path(ex->argv[0], ex->data->env);
	if (!ex->path)
	{
		err_cmd_not_found(ex->argv[0]);
		free_ast(cmd);
		free_env(&ex->data->env);
		child_free_and_exit(ex->argv, NULL, NULL, 127);
	}
}

static void	child_build_envp_and_exec(t_exec *ex, t_ast *cmd)
{
	ex->envp = env_list_to_envp(ex->data->env);
	if (!ex->envp)
	{
		free_ast(cmd);
		free_env(&ex->data->env);
		free(ex->path);
		child_free_and_exit(ex->argv, NULL, ex->path, 1);
	}
	execve(ex->path, ex->argv, ex->envp);
	err_sys_label(ex->path);
	if (errno == ENOENT)
		child_free_and_exit(ex->argv, ex->envp, ex->path, 127);
	else
		child_free_and_exit(ex->argv, ex->envp, ex->path, 126);
}

void	exec_cmd_in_child(t_ast *cmd, t_exec *ex)
{
	child_prepare_redirs(ex, cmd);
	child_prepare_argv(cmd, ex);
	child_try_builtin(ex);
	child_resolve_path(ex, cmd);
	child_build_envp_and_exec(ex, cmd);
}
