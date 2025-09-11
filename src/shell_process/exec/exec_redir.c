#include "minishell.h"

/* NOTE:
 * - L'expand sera ajouté plus tard.
 * - Le strip des quotes se fera ici avant open() quand tu seras prêt.
 * - Pour HEREDOC, run_heredocs_for_redirs préparera r->fd (lecture).
 */

int apply_redirs(t_redir *redirs)
{
	t_redir *r;
	int      fd;

	r = redirs;
	while (r)
	{
		if (r->type == REDIR_IN)
		{
			fd = open(r->filename, O_RDONLY);
			if (fd < 0)
			{
				dprintf(2, "minishell: %s: %s\n", r->filename, strerror(errno));
				return 1;
			}
			if (dup2(fd, STDIN_FILENO) < 0)
			{
				close(fd);
				return 1;
			}
			close(fd);
		}
		else if (r->type == REDIR_OUT)
		{
			fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				dprintf(2, "minishell: %s: %s\n", r->filename, strerror(errno));
				return 1;
			}
			if (dup2(fd, STDOUT_FILENO) < 0)
			{
				close(fd);
				return 1;
			}
			close(fd);
		}
		else if (r->type == REDIR_APPEND)
		{
			fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd < 0)
			{
				dprintf(2, "minishell: %s: %s\n", r->filename, strerror(errno));
				return 1;
			}
			if (dup2(fd, STDOUT_FILENO) < 0)
			{
				close(fd);
				return 1;
			}
			close(fd);
		}
		else if (r->type == REDIR_HEREDOC)
		{
			if (r->fd < 0)
			{
				dprintf(2, "minishell: heredoc: internal error (no fd)\n");
				return 1;
			}
			if (dup2(r->fd, STDIN_FILENO) < 0)
			{
				close(r->fd);
				r->fd = -1;
				return 1;
			}
			close(r->fd);
			r->fd = -1;
		}
		r = r->next;
	}
	return 0;
}

  