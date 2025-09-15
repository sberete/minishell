#include "minishell.h"

// int	apply_redirs(t_redir *r)
// {
// 	int	fd;

// 	while (r)
// 	{
// 		if (r->type == REDIR_IN)
// 		{
// 			if (r->fd >= 0)
// 			{
// 				if (dup2(r->fd, STDIN_FILENO) < 0)
// 					return (1);
// 				close(r->fd);
// 				r->fd = -1;
// 			}
// 			else
// 			{
// 				fd = open(r->filename, O_RDONLY);
// 				if (fd < 0 || dup2(fd, STDIN_FILENO) < 0)
// 				{
// 					if (fd >= 0)
// 						close(fd);
// 					return (1);
// 				}
// 				close(fd);
// 			}
// 		}
// 		else if (r->type == REDIR_OUT || r->type == REDIR_APPEND)
// 		{
// 			int flags = O_CREAT | O_WRONLY;
// 			if (r->type == REDIR_OUT)
// 				flags |= O_TRUNC;
// 			else
// 				flags |= O_APPEND;
// 			fd = open(r->filename, flags, 0644);
// 			if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0)
// 			{
// 				if (fd >= 0)
// 					close(fd);
// 				return (1);
// 			}
// 			close(fd);
// 		}
// 		r = r->next;
// 	}
// 	return (0);
// }
