/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 20:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/12 22:29:25 by sxrimu           ###   ########.fr       */
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
		free_tab(argv);
		child_free_and_exit(data, 127);
	}
	execve(path, argv, envp);
	/* si on revient ici, execve a échoué */
	dprintf(2, "minishell: %s: %s\n", path, strerror(errno));
	if (envp)
		free_envp(envp);
	free(path);
	free_tab(argv);
	if (errno == EACCES)
		child_free_and_exit(data, 126);
	child_free_and_exit(data, 126);
	return (126);
}

/* helper local, visible uniquement dans ce fichier */
static int is_exit_name(const char *s)
{
    return (s && ft_strcmp(s, "exit") == 0);
}

/* ───────── redirs + builtins en parent (argvx déjà expansé, sans fuite de FDs) ───────── */
static int run_builtin_in_parent_with_redirs(t_ast *cmd, char **argvx, t_data *data)
{
    int saved_in  = -1;
    int saved_out = -1;
    int st;

	/* Pas de redirections -> pas de dup, exécuter directement */
	if (cmd->redirs == NULL) {
		if (is_exit_name(argvx[0])) {
			/* ft_exit ne revient pas */
			return ft_exit(argvx, data);
		}
		return run_builtin(argvx, data);
	}

    /* Il y a des redirections -> dupliquer stdin/stdout */
    if (save_stdio(&saved_in, &saved_out) != 0)
        return 1;

    /* Préparer les heredocs avant touchers aux flux (déjà fait, mais ici on les ouvre) */
    if (run_heredocs_for_redirs(cmd->redirs, data) != 0) {
        if (saved_in  >= 0) close(saved_in);
        if (saved_out >= 0) close(saved_out);
        return (int)g_exit_status;
    }

    if (apply_redirs(cmd->redirs) != 0) {
        if (saved_in  >= 0) close(saved_in);
        if (saved_out >= 0) close(saved_out);
        return 1;
    }

    /* Cas spécial: exit — fermer nos dup AVANT de quitter */
    if (is_exit_name(argvx[0])) {
        if (saved_in  >= 0) close(saved_in);
        if (saved_out >= 0) close(saved_out);
        return ft_exit(argvx, data); /* ne revient pas */
    }

    /* Builtin normal */
    st = run_builtin(argvx, data);

    /* Restaurer stdio puis fermer nos dup */
    restore_stdio(saved_in, saved_out);
    return st;
}



/* ───────────────────────── exec d’un simple CMD ───────────────────────── */

static int exec_simple_command(t_ast *cmd, t_data *data, int in_pipeline)
{
	pid_t	pid;
	int		st;
	char  **argvx;

	if (!cmd)
		return (0);

	/* Expansion des redirs en place (filenames) */
	if (expand_redirs_inplace(cmd->redirs, data) != 0)
		return (1);

	/* CMD vide : seulement redirs */
	if (!cmd->argv || !cmd->argv[0])
	{
		if (run_heredocs_for_redirs(cmd->redirs, data) != 0)
			return ((int)g_exit_status);
		if (apply_redirs(cmd->redirs) != 0)
			return (1);
		return (0);
	}

	/* Expansion argv */
	argvx = expand_argv_dup(cmd, data);
	if (!argvx)
		return (1);

	/* builtin en parent si pas de pipeline */
	if (is_builtin(argvx[0]) && !in_pipeline)
	{
		st = run_builtin_in_parent_with_redirs(cmd, argvx, data);
		free_tab(argvx);
		return (st);
	}

	/* sinon: child */
	pid = fork();
	if (pid < 0)
	{
		free_tab(argvx);
		return (1);
	}
	if (pid == 0)
	{
		int s;

		signals_setup_child();

		if (run_heredocs_for_redirs(cmd->redirs, data) != 0)
		{
			free_tab(argvx);
			child_free_and_exit(data, (int)g_exit_status);
		}
		if (apply_redirs(cmd->redirs) != 0)
		{
			free_tab(argvx);
			child_free_and_exit(data, 1);
		}

		if (is_builtin(argvx[0]))
		{
			s = run_builtin(argvx, data);
			free_tab(argvx);
			child_free_and_exit(data, s);
		}
		/* externe */
		/* exec_external_in_child DOIT free argvx si execve échoue */
		exec_external_in_child(argvx, data);
	}
	/* parent */
	free_tab(argvx);
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
