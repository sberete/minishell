// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   heredoc.c                                          :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2025/09/10 20:05:00 by sberete           #+#    #+#             */
// /*   Updated: 2025/09/14 22:44:50 by sberete          ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "minishell.h"
// #include <errno.h>
// #include <fcntl.h>
// #include <readline/history.h>
// #include <readline/readline.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <unistd.h>

// /* Ecrit tout le heredoc dans wfd. Retour:
//    0   => OK
//    1   => erreur write/malloc */
// static int	heredoc_writer_loop(int wfd, const char *raw_delim, int do_expand,
// 		t_data *data)
// {
// 	char	*lim;
// 	char	*line;
// 	char	*out;

// 	lim = hd_strip_outer_quotes(raw_delim);
// 	if (!lim)
// 		return (1);
// 	while (1)
// 	{
// 		line = readline("> ");
// 		if (!line)
// 			break ;
// 		if (ft_strcmp(line, lim) == 0)
// 		{
// 			free(line);
// 			break ;
// 		}
// 		if (do_expand)
// 			out = heredoc_expand_line(line, data);
// 		else
// 			out = ft_strdup(line);
// 		if (!out)
// 		{
// 			free(line);
// 			free(lim);
// 			return (1);
// 		}
// 		if (write(wfd, out, ft_strlen(out)) < 0 || write(wfd, "\n", 1) < 0)
// 		{
// 			free(out);
// 			free(line);
// 			free(lim);
// 			return (1);
// 		}
// 		free(out);
// 		free(line);
// 	}
// 	free(lim);
// 	return (0);
// }

// /* Construit r->fd en lisant le heredoc dans un child qui écrit sur un pipe */
// static int	build_heredoc_fd(t_redir *r, t_data *data)
// {
// 	int		p[2];
// 	pid_t	pid;
// 	int		st;
// 		int rc;

// 	if (pipe(p) < 0)
// 		return (1);
// 	/* Parent: ignore INT/QUIT pendant que le child lit */
// 	signals_setup_heredoc_parent();
// 	pid = fork();
// 	if (pid < 0)
// 	{
// 		close(p[0]);
// 		close(p[1]);
// 		signals_setup_parent(); /* rétablit le handler readline */
// 		return (1);
// 	}
// 	if (pid == 0)
// 	{
// 		/* CHILD: défaut SIGINT/SIGQUIT (ton choix),
// 			readline fera interrompre le process */
// 		signals_setup_heredoc_child();
// 		close(p[0]);
// 		rc = heredoc_writer_loop(p[1], r->delim, r->delim_can_expand ? 1 : 0,
// 				data);
// 		close(p[1]);
// 		/* Nettoyage côté child pour éviter still-reachable quand on sort "normalement" */
// 		child_free_and_exit(data, rc != 0 ? 1 : 0);
// 	}
// 	/* PARENT */
// 	close(p[1]);
// 	/* On attend le child; update_exit_from_wait() met g_exit_status et écrit "\n"/"Quit ..." */
// 	st = wait_and_update_exit(pid);
// 	/* Rétablir les handlers parent interactifs (rl_replace_line etc.) */
// 	signals_setup_parent();
// 	if (st == 130)
// 	{
// 		/* interrompu par ^C : on ferme et on propage l’annulation 130 */
// 		if (p[0] >= 0)
// 			close(p[0]);
// 		g_exit_status = 130;
// 		return (130);
// 	}
// 	if (st != 0)
// 	{
// 		if (p[0] >= 0)
// 			close(p[0]);
// 		return (1);
// 	}
// 	/* OK: r->fd reçoit le côté lecture du pipe (sera dup2() vers STDIN) */
// 	r->fd = p[0];
// 	(void)fcntl(r->fd, F_SETFD, FD_CLOEXEC);
// 	return (0);
// }

// /* Parcourt toutes les redirs et prépare les heredocs (met r->fd) */
// int	run_heredocs_for_redirs(t_redir *redirs, t_data *data)
// {
// 	t_redir	*r;
// 	int		rc;

// 	r = redirs;
// 	while (r)
// 	{
// 		if (r->type == REDIR_HEREDOC)
// 		{
// 			if (r->fd >= 0)
// 			{
// 				close(r->fd);
// 				r->fd = -1;
// 			}
// 			rc = build_heredoc_fd(r, data);
// 			if (rc != 0)
// 				return (rc); /* 130 si ^C, 1 si erreur */
// 		}
// 		r = r->next;
// 	}
// 	return (0);
// }
