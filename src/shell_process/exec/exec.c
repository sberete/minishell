// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   exec.c                                             :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2025/08/25 21:59:32 by sberete           #+#    #+#             */
// /*   Updated: 2025/09/02 20:07:58 by sberete          ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "minishell.h"

// if (is_builtin(argv[0]))
//     run_builtin(argv, data);
// else
//     run_external(argv, data->env);

// void	init_exec(t_exec *exec, t_ast *node, int prev_fd, t_data *data)
// {
// 	exec->prev_read = prev_fd;
// 	exec->pipe_fd[0] = -1;
// 	exec->pipe_fd[1] = -1;
// 	exec->argv = node->argv;
// 	exec->redirs = node->redirs;
// 	exec->path = NULL;
// 	exec->pids = NULL;
// 	exec->data = data;
// }

// pid_t	exec_command(t_exec *exec, int is_last)
// {
// 	pid_t	pid;

// 	if (!exec || !exec->argv || !exec->argv[0])
// 		exit(127);
// 	if (!is_last && pipe(exec->pipe_fd) == -1)
// 		perror("pipe");
// 	pid = fork();
// 	if (pid == -1)
// 	{
// 		perror("fork");
// 		return (-1);
// 	}
// 	if (pid == 0) // --- Fils ---
// 	{
// 		/* si une entrée précédente existe */
// 		if (exec->prev_read != -1)
// 		{
// 			dup2(exec->prev_read, STDIN_FILENO);
// 			close(exec->prev_read);
// 		}
// 		/* si ce n’est pas la dernière commande, on prépare la sortie */
// 		if (!is_last)
// 		{
// 			dup2(exec->pipe_fd[1], STDOUT_FILENO);
// 		}
// 		/* fermer les FDs inutiles dans le fils */
// 		if (exec->pipe_fd[0] != -1)
// 			close(exec->pipe_fd[0]);
// 		if (exec->pipe_fd[1] != -1)
// 			close(exec->pipe_fd[1]);
// 		apply_redirs(exec->redirs);
// 		exec->path = resolve_path(exec->argv[0], exec->data->env);
// 		if (exec->path)
// 			execve(exec->path, exec->argv, exec->data->env);
// 		if (access(exec->argv[0], X_OK) == 0)
// 			execve(exec->argv[0], exec->argv, exec->data->env);
// 		perror(exec->argv[0]);
// 		exit(127);
// 	}
// 	// --- Parent ---
// 	if (exec->prev_read != -1)
// 		close(exec->prev_read);
// 	if (!is_last)
// 		close(exec->pipe_fd[1]);
// 	return (pid);
// }

// int	exec_pipe(t_ast *node, t_data *data)
// {
// 	t_exec	exec;
// 	int		prev_fd;
// 	int		i;
// 	int		count;
// 	pid_t	*pids;
// 	int		status;
// 	t_ast	*tmp;

// 	prev_fd = -1;
// 	i = 0;
// 	count = 0;
// 	pids = NULL;
// 	status = 0;
// 	/* Compter le nombre de commandes */
// 	tmp = node;
// 	while (tmp && tmp->type == NODE_PIPE)
// 	{
// 		count++;
// 		tmp = tmp->right;
// 	}
// 	count++; // dernière commande
// 	pids = malloc(sizeof(pid_t) * count);
// 	if (!pids)
// 		return (1);
// 	/* Réinitialiser tmp pour exécution */
// 	tmp = node;
// 	i = 0;
// 	while (tmp && tmp->type == NODE_PIPE)
// 	{
// 		init_exec(&exec, tmp->left, prev_fd, data);
// 		exec.pids = pids;
// 		pids[i++] = exec_command(&exec, 0);
// 		prev_fd = exec.pipe_fd[0];
// 		tmp = tmp->right;
// 	}
// 	if (tmp) // dernière commande
// 	{
// 		init_exec(&exec, tmp, prev_fd, data);
// 		exec.pids = pids;
// 		pids[i++] = exec_command(&exec, 1);
// 	}
// 	for (int j = 0; j < i; j++)
// 		waitpid(pids[j], &status, 0);
// 	free(pids);
// 	return (WEXITSTATUS(status));
// }

// int	exec_ast(t_ast *node, t_data *data)
// {
// 	t_exec	exec;
// 	pid_t	pid;
// 	int		status;
// 	int		left_status;

// 	if (!node)
// 		return (0);
// 	if (node->type == NODE_CMD)
// 	{
// 		init_exec(&exec, node, -1, data);
// 		pid = exec_command(&exec, 1);
// 		waitpid(pid, &status, 0);
// 		return (WEXITSTATUS(status));
// 	}
// 	if (node->type == NODE_PIPE)
// 		return (exec_pipe(node, data));
// 	if (node->type == NODE_AND)
// 	{
// 		left_status = exec_ast(node->left, data);
// 		if (left_status == 0)
// 			return (exec_ast(node->right, data));
// 		return (left_status);
// 	}
// 	if (node->type == NODE_OR)
// 	{
// 		left_status = exec_ast(node->left, data);
// 		if (left_status != 0)
// 			return (exec_ast(node->right, data));
// 		return (left_status);
// 	}
// 	if (node->type == NODE_SEQ)
// 	{
// 		exec_ast(node->left, data);
// 		return (exec_ast(node->right, data));
// 	}
// 	if (node->type == NODE_GROUP)
// 	{
// 		pid = fork();
// 		if (pid == 0)
// 		{
// 			status = exec_ast(node->child, data);
// 			exit(status);
// 		}
// 		waitpid(pid, &status, 0);
// 		return (WEXITSTATUS(status));
// 	}
// 	return (0);
// }
