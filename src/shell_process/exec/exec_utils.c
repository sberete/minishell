/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 20:45:00 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:41:23 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/* ───────────────────────── Child cleanup ───────────────────────── */

void	child_free_and_exit(t_data *data, int status)
{
	/* On libère la copie du child (safe: COW). */
	if (data)
	{
		if (data->ast)
		{
			free_ast(data->ast);
			data->ast = NULL;
		}
		if (data->tokens)
		{
			free_token_list(&data->tokens);
			data->tokens = NULL;
		}
		if (data->line)
		{
			free(data->line);
			data->line = NULL;
		}
		if (data->env)
		{
			env_clear(&data->env);
		}
	}
	_exit(status);
}

/* ───────────────────────── PATH & envp ───────────────────────── */

char	**env_list_to_envp(t_env *lst)
{
	size_t	n;
	char	**envp;
	size_t	i;
	size_t	klen;
	size_t	vlen;

	n = env_size(lst);
	envp = (char **)malloc(sizeof(char *) * (n + 1));
	if (!envp)
		return (NULL);
	i = 0;
	while (lst)
	{
		klen = ft_strlen(lst->key);
		if (lst->value)
			vlen = ft_strlen(lst->value);
		else
			vlen = 0;
		envp[i] = (char *)malloc(klen + 1 + vlen + 1);
		if (!envp[i])
		{
			free_envp(envp);
			return (NULL);
		}
		ft_memcpy(envp[i], lst->key, klen);
		envp[i][klen] = '=';
		if (lst->value)
			ft_memcpy(envp[i] + klen + 1, lst->value, vlen);
		envp[i][klen + 1 + vlen] = '\0';
		lst = lst->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

void	free_envp(char **envp)
{
	size_t i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}

char	*find_cmd_path(char *cmd, t_env *envlst)
{
	const char	*path;
	char		**dirs;
	size_t		i;
	char		*full;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path = ms_getenv(envlst, "PATH");
	if (!path)
		return (NULL);
	dirs = ft_split(path, ':');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		full = ft_pathjoin(dirs[i], cmd);
		if (full)
		{
			if (access(full, X_OK) == 0)
			{
				free_tab(dirs);
				return (full);
			}
			free(full);
		}
		i++;
	}
	free_tab(dirs);
	return (NULL);
}

/* ───────────────────────── Wait helpers ───────────────────────── */

int	wait_and_update_exit(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) < 0)
		return (1);
	update_exit_from_wait(status); /* met g_exit_status et affiche ^C/QUIT */
	return ((int)g_exit_status);
}

/* ───────────────────────── Redir helpers (parent) ───────────────────────── */

int	save_stdio(int *saved_in, int *saved_out)
{
	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	if (*saved_in < 0 || *saved_out < 0)
	{
		if (*saved_in >= 0)
			close(*saved_in);
		if (*saved_out >= 0)
			close(*saved_out);
		return (1);
	}
	return (0);
}

void	restore_stdio(int saved_in, int saved_out)
{
	if (saved_in >= 0)
	{
		if (dup2(saved_in, STDIN_FILENO) >= 0)
			close(saved_in);
		else
			close(saved_in);
	}
	if (saved_out >= 0)
	{
		if (dup2(saved_out, STDOUT_FILENO) >= 0)
			close(saved_out);
		else
			close(saved_out);
	}
}
