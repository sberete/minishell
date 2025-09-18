// #include "minishell.h"
// #include <unistd.h>
// #include <stdlib.h>

// static void	set_fd_pair(int fd[2], int a, int b)
// {
// 	fd[0] = a;
// 	fd[1] = b;
// }

// static void	close_if_open(int *fd)
// {
// 	if (*fd >= 0)
// 	{
// 		close(*fd);
// 		*fd = -1;
// 	}
// }

// void	exec_ctx_init_cmd(t_exec *x, t_data *d, t_ast *cmd)
// {
// 	if (!x)
// 		return ;
// 	x->prev_read = -1;
// 	set_fd_pair(x->pipe_fd, -1, -1);
// 	set_fd_pair(x->fd_heredoc, -1, -1);
// 	x->path = NULL;
// 	x->argv = NULL;
// 	x->envp = NULL;
// 	x->redirs = cmd ? cmd->redirs : NULL;
// 	x->data = d;
// }

// void	exec_ctx_close_pipes(t_exec *x)
// {
// 	if (!x)
// 		return ;
// 	close_if_open(&x->prev_read);
// 	close_if_open(&x->pipe_fd[0]);
// 	close_if_open(&x->pipe_fd[1]);
// 	close_if_open(&x->fd_heredoc[0]);
// 	close_if_open(&x->fd_heredoc[1]);
// }

// void	exec_ctx_free_argv(t_exec *x)
// {
// 	size_t	i;

// 	if (!x || !x->argv)
// 		return ;
// 	i = 0;
// 	while (x->argv[i])
// 	{
// 		free(x->argv[i]);
// 		i++;
// 	}
// 	free(x->argv);
// 	x->argv = NULL;
// }

// void	exec_ctx_free_envp(t_exec *x)
// {
// 	size_t	i;

// 	if (!x || !x->envp)
// 		return ;
// 	i = 0;
// 	while (x->envp[i])
// 	{
// 		free(x->envp[i]);
// 		i++;
// 	}
// 	free(x->envp);
// 	x->envp = NULL;
// }

// void	exec_ctx_free_path(t_exec *x)
// {
// 	if (!x || !x->path)
// 		return ;
// 	free(x->path);
// 	x->path = NULL;
// }
