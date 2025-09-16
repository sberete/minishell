#include "minishell.h"
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/* 1/5 */
static int	dup2_close(int src, int dst, const char *ctx)
{
	if (src < 0)
		return (0);
	if (dup2(src, dst) < 0)
	{
		fprintf(stderr, "minishell: %s: ", ctx);
		perror(NULL);
		close(src);
		return (1);
	}
	close(src);
	return (0);
}

/* 2/5 */
static int	open_out_fd(const t_redir *r)
{
	int	flags;

	flags = O_CREAT | O_WRONLY;
	if (r->type == REDIR_OUT)
		flags |= O_TRUNC;
	else
		flags |= O_APPEND;
	return (open(r->filename, flags, 0644));
}

/* 3/5 : pour pipeline: duplique in_fd/out_fd si fournis */
int	redirect_io(int in_fd, int out_fd)
{
	if (in_fd >= 0)
	{
		if (dup2_close(in_fd, STDIN_FILENO, "dup2 stdin") != 0)
			return (1);
	}
	if (out_fd >= 0)
	{
		if (dup2_close(out_fd, STDOUT_FILENO, "dup2 stdout") != 0)
			return (1);
	}
	return (0);
}

/* 4/5 : applique la liste de redirs (heredoc via r->fd, sinon open) */
int	apply_redirs(t_redir *r)
{
	int	fd;

	while (r)
	{
		if (r->type == REDIR_IN || r->type == REDIR_HEREDOC)
		{
			if (r->fd >= 0)
			{
				if (dup2_close(r->fd, STDIN_FILENO, "dup2 heredoc") != 0)
					return (1);
				r->fd = -1;
			}
			else
			{
				fd = open(r->filename, O_RDONLY);
				if (fd < 0)
				{
					fprintf(stderr, "minishell: %s: ", r->filename);
					perror(NULL);
					return (1);
				}
				if (dup2_close(fd, STDIN_FILENO, "dup2 <") != 0)
					return (1);
			}
		}
		else if (r->type == REDIR_OUT || r->type == REDIR_APPEND)
		{
			fd = open_out_fd(r);
			if (fd < 0)
			{
				fprintf(stderr, "minishell: %s: ", r->filename);
				perror(NULL);
				return (1);
			}
			if (dup2_close(fd, STDOUT_FILENO, "dup2 > / >>") != 0)
				return (1);
		}
		r = r->next;
	}
	return (0);
}

/* 5/5 : effets de bord uniquement (redir seule sans modifier les fds) */
int	redirs_touch_only(t_redir *r)
{
	int	fd;

	while (r)
	{
		if (r->type == REDIR_OUT || r->type == REDIR_APPEND)
		{
			fd = open_out_fd(r);
			if (fd < 0)
			{
				fprintf(stderr, "minishell: %s: ", r->filename);
				perror(NULL);
				return (1);
			}
			close(fd);
		}
		else if (r->type == REDIR_IN)
		{
			fd = open(r->filename, O_RDONLY);
			if (fd < 0)
			{
				fprintf(stderr, "minishell: %s: ", r->filename);
				perror(NULL);
				return (1);
			}
			close(fd);
		}
		else if (r->type == REDIR_HEREDOC && r->fd >= 0)
		{
			close(r->fd);
			r->fd = -1;
		}
		r = r->next;
	}
	return (0);
}
