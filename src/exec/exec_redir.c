#include "minishell.h"

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