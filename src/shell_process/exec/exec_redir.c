// #include "minishell.h"

// /* NOTE:
//  * - L'expand sera ajouté plus tard.
//  * - Le strip des quotes se fera ici avant open() quand tu seras prêt.
//  * - Pour HEREDOC, run_heredocs_for_redirs préparera r->fd (lecture).
//  */

// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   exec_redir.c                                       :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2025/09/15 00:45:00 by sberete           #+#    #+#             */
// /*   Updated: 2025/09/15 00:45:00 by sberete          ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "minishell.h"
// #include <fcntl.h>
// #include <unistd.h>

// static int	open_infile(const char *path)
// {
// 	if (!path)
// 		return (-1);
// 	return (open(path, O_RDONLY));
// }

// static int	open_outfile_trunc(const char *path)
// {
// 	if (!path)
// 		return (-1);
// 	return (open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644));
// }

// static int	open_outfile_append(const char *path)
// {
// 	if (!path)
// 		return (-1);
// 	return (open(path, O_WRONLY | O_CREAT | O_APPEND, 0644));
// }

// static int	apply_one_redir(t_redir *r)
// {
// 	int	fd;

// 	if (r->type == REDIR_IN)
// 	{
// 		fd = open_infile(r->filename);
// 		if (fd < 0 || dup2_and_close(fd, STDIN_FILENO) != 0)
// 			return (-1);
// 		return (0);
// 	}
// 	if (r->type == REDIR_OUT)
// 	{
// 		fd = open_outfile_trunc(r->filename);
// 		if (fd < 0 || dup2_and_close(fd, STDOUT_FILENO) != 0)
// 			return (-1);
// 		return (0);
// 	}
// 	if (r->type == REDIR_APPEND)
// 	{
// 		fd = open_outfile_append(r->filename);
// 		if (fd < 0 || dup2_and_close(fd, STDOUT_FILENO) != 0)
// 			return (-1);
// 		return (0);
// 	}
// 	if (dup2_and_close(r->fd, STDIN_FILENO) != 0)
// 		return (-1);
// 	r->fd = -1;
// 	return (0);
// }

// int	apply_redirections(t_redir *rlist)
// {
// 	while (rlist)
// 	{
// 		if (apply_one_redir(rlist) != 0)
// 			return (-1);
// 		rlist = rlist->next;
// 	}
// 	return (0);
// }

