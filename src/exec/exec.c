/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:59:32 by sberete           #+#    #+#             */
/*   Updated: 2025/08/25 22:04:02 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Récupère la variable PATH et la split en tableau */
char	**get_env_path(char **env)
{
	int		i;
	char	*path_var;

	i = 0;
	path_var = NULL;
	while (env && env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
		{
			path_var = env[i] + 5;
			break ;
		}
		i++;
	}
	if (!path_var)
		return (NULL);
	return (ft_split(path_var, ':'));
}

/* Résout le chemin complet d'une commande */
char	*resolve_path(char *cmd, char **env)
{
	char	**paths;
	char	*full;
	int		i;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return ((access(cmd, X_OK) == 0) ? ft_strdup(cmd) : NULL);
	paths = get_env_path(env);
	i = 0;
	while (paths && paths[i])
	{
		full = ft_pathjoin(paths[i], cmd);
		if (access(full, X_OK) == 0)
		{
			free_tab(paths);
			return (full);
		}
		free(full);
		i++;
	}
	free_tab(paths);
	return (NULL);
}

/* Applique les redirections d'un nœud */
void	apply_redirs(t_redir *redirs)
{
	int	fd;

	while (redirs)
	{
		if (redirs->type == REDIR_IN)
			fd = open(redirs->filename, O_RDONLY);
		else if (redirs->type == REDIR_OUT)
			fd = open(redirs->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (redirs->type == REDIR_APPEND)
			fd = open(redirs->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd < 0)
		{
			perror(redirs->filename);
			exit(1);
		}
		else
		{
			printf("[DEBUG] %s ouvert avec fd=%d (type=%d)\n", redirs->filename,
				fd, redirs->type);
		}
		if (redirs->type == REDIR_IN || redirs->type == REDIR_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		redirs = redirs->next;
	}
}

/* Initialise la structure d'exécution */
void	init_exec(t_exec *exec, t_ast *node, int prev_fd, t_data *data)
{
	exec->prev_read = prev_fd;
	exec->pipe_fd[0] = -1;
	exec->pipe_fd[1] = -1;
	exec->argv = node->argv;
	exec->redirs = node->redirs;
	exec->path = NULL;
	exec->pids = NULL;
	exec->data = data;
}

/* Exécute une commande simple et renvoie son pid */
pid_t	exec_command(t_exec *exec, int is_last)
{
	pid_t	pid;

	if (!exec || !exec->argv || !exec->argv[0])
		exit(127);
	if (!is_last && pipe(exec->pipe_fd) == -1)
		perror("pipe");
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0) // --- Fils ---
	{
		/* si une entrée précédente existe */
		if (exec->prev_read != -1)
		{
			dup2(exec->prev_read, STDIN_FILENO);
			close(exec->prev_read);
		}
		/* si ce n’est pas la dernière commande, on prépare la sortie */
		if (!is_last)
		{
			dup2(exec->pipe_fd[1], STDOUT_FILENO);
		}
		/* fermer les FDs inutiles dans le fils */
		if (exec->pipe_fd[0] != -1)
			close(exec->pipe_fd[0]);
		if (exec->pipe_fd[1] != -1)
			close(exec->pipe_fd[1]);
		apply_redirs(exec->redirs);
		exec->path = resolve_path(exec->argv[0], exec->data->env);
		if (exec->path)
			execve(exec->path, exec->argv, exec->data->env);
		if (access(exec->argv[0], X_OK) == 0)
			execve(exec->argv[0], exec->argv, exec->data->env);
		perror(exec->argv[0]);
		exit(127);
	}
	// --- Parent ---
	if (exec->prev_read != -1)
		close(exec->prev_read);
	if (!is_last)
		close(exec->pipe_fd[1]);
	return (pid);
}

/* Exécute une séquence de pipes */
int	exec_pipe(t_ast *node, t_data *data)
{
	t_exec	exec;
	int		prev_fd;
	int		i;
	int		count;
	pid_t	*pids;
	int		status;
	t_ast	*tmp;

	prev_fd = -1;
	i = 0;
	count = 0;
	pids = NULL;
	status = 0;
	/* Compter le nombre de commandes */
	tmp = node;
	while (tmp && tmp->type == NODE_PIPE)
	{
		count++;
		tmp = tmp->right;
	}
	count++; // dernière commande
	pids = malloc(sizeof(pid_t) * count);
	if (!pids)
		return (1);
	/* Réinitialiser tmp pour exécution */
	tmp = node;
	i = 0;
	while (tmp && tmp->type == NODE_PIPE)
	{
		init_exec(&exec, tmp->left, prev_fd, data);
		exec.pids = pids;
		pids[i++] = exec_command(&exec, 0);
		prev_fd = exec.pipe_fd[0];
		tmp = tmp->right;
	}
	if (tmp) // dernière commande
	{
		init_exec(&exec, tmp, prev_fd, data);
		exec.pids = pids;
		pids[i++] = exec_command(&exec, 1);
	}
	/* Attendre tous les fils */
	for (int j = 0; j < i; j++)
		waitpid(pids[j], &status, 0);
	free(pids);
	return (WEXITSTATUS(status));
}

/* Exécute l'AST selon le type de nœud */
int	exec_ast(t_ast *node, t_data *data)
{
	t_exec	exec;
	pid_t	pid;
	int		status;
	int		left_status;

	if (!node)
		return (0);
	if (node->type == NODE_CMD)
	{
		init_exec(&exec, node, -1, data);
		pid = exec_command(&exec, 1);
		waitpid(pid, &status, 0);
		return (WEXITSTATUS(status));
	}
	if (node->type == NODE_PIPE)
		return (exec_pipe(node, data));
	if (node->type == NODE_AND)
	{
		left_status = exec_ast(node->left, data);
		if (left_status == 0)
			return (exec_ast(node->right, data));
		return (left_status);
	}
	if (node->type == NODE_OR)
	{
		left_status = exec_ast(node->left, data);
		if (left_status != 0)
			return (exec_ast(node->right, data));
		return (left_status);
	}
	if (node->type == NODE_SEQ)
	{
		exec_ast(node->left, data);
		return (exec_ast(node->right, data));
	}
	if (node->type == NODE_GROUP)
	{
		pid = fork();
		if (pid == 0)
		{
			status = exec_ast(node->child, data);
			exit(status);
		}
		waitpid(pid, &status, 0);
		return (WEXITSTATUS(status));
	}
	return (0);
}
