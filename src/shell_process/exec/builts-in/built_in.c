#include "minishell.h"
/* ---------- Builtins ---------- */

int is_builtin(const char *name)
{
	if (!name)
		return 0;
	if (name[0] == '\0')
		return 0;
	if (ft_strcmp(name, "echo") == 0) return 1;
	if (ft_strcmp(name, "cd") == 0) return 1;
	if (ft_strcmp(name, "pwd") == 0) return 1;
	if (ft_strcmp(name, "export") == 0) return 1;
	if (ft_strcmp(name, "unset") == 0) return 1;
	if (ft_strcmp(name, "env") == 0) return 1;
	if (ft_strcmp(name, "exit") == 0) return 1;
	return 0;
}

int run_builtin(char **argv, t_data *data)
{
	if (!argv || !argv[0])
		return 0;

	if (ft_strcmp(argv[0], "echo") == 0)   return ft_echo(argv, data);
	if (ft_strcmp(argv[0], "cd") == 0)     return ft_cd(argv, data);
	if (ft_strcmp(argv[0], "pwd") == 0)    return ft_pwd(argv, data);
	if (ft_strcmp(argv[0], "export") == 0) return ft_export(argv, data);
	if (ft_strcmp(argv[0], "unset") == 0)  return ft_unset(argv, data);
	if (ft_strcmp(argv[0], "env") == 0)    return ft_env(argv, data);
	if (ft_strcmp(argv[0], "exit") == 0)   return ft_exit(argv, data);

	return 1;
}


/* Builtins à exécuter dans le parent (si pas de pipe) */
int run_builtin_parent(t_ast *cmd, t_data *data)
{
	int saved_stdin;
	int saved_stdout;
	int status;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin < 0 || saved_stdout < 0)
		return 1;

	if (run_heredocs_for_redirs(cmd->redirs, data) != 0)
	{
		if (saved_stdin >= 0)
			close(saved_stdin);
		if (saved_stdout >= 0)
			close(saved_stdout);
		return g_exit_status;
	}
	if (apply_redirs(cmd->redirs) != 0)
	{
		if (saved_stdin >= 0)
			close(saved_stdin);
		if (saved_stdout >= 0)
			close(saved_stdout);
		return 1;
	}

	status = run_builtin(cmd->argv, data);

	if (dup2(saved_stdin, STDIN_FILENO) < 0) {}
	if (dup2(saved_stdout, STDOUT_FILENO) < 0) {}
	if (saved_stdin >= 0)
		close(saved_stdin);
	if (saved_stdout >= 0)
		close(saved_stdout);

	return status;
}
