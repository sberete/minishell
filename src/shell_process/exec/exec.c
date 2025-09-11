/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 20:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:50:57 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/* ───────────────────────── extern ───────────────────────── */

static int exec_external_in_child(char **argv, t_data *data)
{
	char	**envp;
	char	*path;

	envp = env_list_to_envp(data->env);
	path = find_cmd_path(argv[0], data->env);
	if (!path)
	{
		dprintf(2, "minishell: %s: command not found\n", argv[0]);
		if (envp)
			free_envp(envp);
		child_free_and_exit(data, 127);
	}
	execve(path, argv, envp);
	/* si on revient ici, execve a échoué */
	dprintf(2, "minishell: %s: %s\n", path, strerror(errno));
	if (envp)
		free_envp(envp);
	free(path);
	if (errno == EACCES)
		child_free_and_exit(data, 126);
	child_free_and_exit(data, 126);
	return (126);
}

/* ───────────────────────── redirs builtins en parent ───────────────────────── */

static int run_builtin_in_parent_with_redirs(t_ast *cmd, t_data *data)
{
	int saved_in;
	int saved_out;
	int st;

	if (save_stdio(&saved_in, &saved_out) != 0)
		return (1);
	/* Prépare heredocs avant les dup */
	if (run_heredocs_for_redirs(cmd->redirs, data) != 0)
	{
		restore_stdio(saved_in, saved_out);
		return ((int)g_exit_status);
	}
	if (apply_redirs(cmd->redirs) != 0)
	{
		restore_stdio(saved_in, saved_out);
		return (1);
	}
	st = run_builtin(cmd->argv, data);
	restore_stdio(saved_in, saved_out);
	return (st);
}

/* ───────────────────────── exec d’un simple CMD ───────────────────────── */

static int exec_simple_command(t_ast *cmd, t_data *data, int in_pipeline)
{
	pid_t	pid;
	int		st;

	if (!cmd)
		return (0);

	/* CMD vide : juste redirs (création de fichiers, dernier stdin si heredoc) */
	if (!cmd->argv || !cmd->argv[0])
	{
		if (run_heredocs_for_redirs(cmd->redirs, data) != 0)
			return ((int)g_exit_status);
		if (apply_redirs(cmd->redirs) != 0)
			return (1);
		return (0);
	}

	/* builtin en parent si pas de pipeline */
	if (is_builtin(cmd->argv[0]) && !in_pipeline)
		return (run_builtin_in_parent_with_redirs(cmd, data));

	/* sinon: forker et tout faire en enfant */
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		int s;

		signals_setup_child();

		if (run_heredocs_for_redirs(cmd->redirs, data) != 0)
			child_free_and_exit(data, (int)g_exit_status);
		if (apply_redirs(cmd->redirs) != 0)
			child_free_and_exit(data, 1);

		if (is_builtin(cmd->argv[0]))
		{
			s = run_builtin(cmd->argv, data);
			child_free_and_exit(data, s);
		}
		exec_external_in_child(cmd->argv, data);
	}
	/* parent */
	st = wait_and_update_exit(pid);
	return (st);
}

/* ───────────────────────── pipeline (binaire) ───────────────────────── */

static int exec_pipeline(t_ast *node, t_data *data)
{
	int		p[2];
	pid_t	lp;
	pid_t	rp;
	int		status_right;

	if (pipe(p) < 0)
		return (1);

	lp = fork();
	if (lp < 0)
	{
		close(p[0]);
		close(p[1]);
		return (1);
	}
	if (lp == 0)
	{
		int s;

		signals_setup_child();
		if (dup2(p[1], STDOUT_FILENO) < 0)
			child_free_and_exit(data, 1);
		close(p[0]);
		close(p[1]);
		s = exec_simple_command(node->left, data, 1);
		child_free_and_exit(data, s);
	}

	rp = fork();
	if (rp < 0)
	{
		close(p[0]);
		close(p[1]);
		waitpid(lp, NULL, 0);
		return (1);
	}
	if (rp == 0)
	{
		int s;

		signals_setup_child();
		if (dup2(p[0], STDIN_FILENO) < 0)
			child_free_and_exit(data, 1);
		close(p[0]);
		close(p[1]);
		s = exec_simple_command(node->right, data, 1);
		child_free_and_exit(data, s);
	}

	/* parent */
	close(p[0]);
	close(p[1]);
	/* on ignore le code exact du left; on renvoie celui de right */
	waitpid(lp, NULL, 0);
	status_right = wait_and_update_exit(rp);
	return (status_right);
}

/* ───────────────────────── subshell (group) ───────────────────────── */

static int exec_group_subshell(t_ast *node, t_data *data)
{
	pid_t	pid;
	int		st;

	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		int s;

		signals_setup_child();
		s = exec_ast(node->child, data);
		child_free_and_exit(data, s);
	}
	st = wait_and_update_exit(pid);
	return (st);
}

/* ───────────────────────── dispatcher récursif ───────────────────────── */

static int exec_node(t_ast *node, t_data *data, int in_pipeline)
{
	int ls;

	if (!node)
		return (0);
	if (node->type == NODE_CMD)
		return (exec_simple_command(node, data, in_pipeline));
	if (node->type == NODE_PIPE)
		return (exec_pipeline(node, data));
	if (node->type == NODE_AND)
	{
		ls = exec_node(node->left, data, 0);
		if (ls == 0)
			return (exec_node(node->right, data, 0));
		return (ls);
	}
	if (node->type == NODE_OR)
	{
		ls = exec_node(node->left, data, 0);
		if (ls != 0)
			return (exec_node(node->right, data, 0));
		return (ls);
	}
	if (node->type == NODE_SEQ)
	{
		(void)exec_node(node->left, data, 0);
		return (exec_node(node->right, data, 0));
	}
	if (node->type == NODE_GROUP)
		return (exec_group_subshell(node, data));
	return (0);
}

/* ───────────────────────── Entrée publique ───────────────────────── */

int	exec_ast(t_ast *node, t_data *data)
{
	int status;

	status = exec_node(node, data, 0);
	g_exit_status = status;
	data->last_exit = status;
	return (status);
}
