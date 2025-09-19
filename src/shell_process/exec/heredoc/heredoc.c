/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:39:52 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 02:19:13 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*hd_strip_outer_quotes(char *s)
{
	size_t	len;
	char	q;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	if (len >= 2)
	{
		q = s[0];
		if ((q == '\'' || q == '\"') && s[len - 1] == q)
			return (substr_dup(s, 1, len - 2));
	}
	return (ft_strdup(s));
}

static int	write_all(int fd, const char *s, size_t n)
{
	ssize_t	w;
	size_t	i;

	i = 0;
	while (i < n)
	{
		w = write(fd, s + i, n - i);
		if (w < 0)
			return (-1);
		i += (size_t)w;
	}
	return (0);
}

static int	heredoc_loop(int wfd, const char *delim, int do_expand, t_data *d)
{
	char	*line;
	char	*out;

	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			break ;
		}
		if (do_expand)
			out = ms_expand_vars(line, d);
		else
			out = ft_strdup(line);
		free(line);
		if (!out)
			return (-1);
		if (write_all(wfd, out, ft_strlen(out)) != 0 || write_all(wfd, "\n",
				1) != 0)
		{
			free(out);
			return (-1);
		}
		free(out);
	}
	return (0);
}

static int	run_one_heredoc_child(int wfd, t_redir *r, t_data *d)
{
	char	*plain;
	int		do_expand;
	int		rc;

	signals_setup_heredoc_child();
	do_expand = 0;
	if (r->delim_can_expand)
		do_expand = 1;
	plain = hd_strip_outer_quotes(r->delim);
	if (!plain)
		return (1);
	rc = heredoc_loop(wfd, plain, do_expand, d);
	free(plain);
	if (rc != 0)
		return (1);
	return (0);
}

static int	run_one_heredoc(t_redir *r, t_data *d)
{
	int		pfd[2];
	pid_t	pid;
	int		st;

	if (pipe(pfd) != 0)
		return (1);
	pid = fork();
	if (pid < 0)
	{
		close(pfd[0]);
		close(pfd[1]);
		return (1);
	}
	if (pid == 0)
	{
		close(pfd[0]);
		if (run_one_heredoc_child(pfd[1], r, d) != 0)
			_exit(1);
		close(pfd[1]);
		_exit(0);
	}
	signals_setup_heredoc_parent();
	close(pfd[1]);
	if (waitpid(pid, &st, 0) < 0)
	{
		close(pfd[0]);
		return (1);
	}
	if (WIFSIGNALED(st))
	{
		close(pfd[0]);
		d->last_exit = 130;
		return (1);
	}
	if (WIFEXITED(st) && WEXITSTATUS(st) != 0)
	{
		close(pfd[0]);
		return (1);
	}
	r->fd = pfd[0];
	r->type = REDIR_IN;
	if (r->delim)
	{
		free(r->delim);
		r->delim = NULL;
	}
	return (0);
}

int	run_heredocs_for_redirs(t_redir *r, t_data *d)
{
	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			if (run_one_heredoc(r, d) != 0)
				return (1);
		}
		r = r->next;
	}
	return (0);
}

/* parcours l’AST et prépare tous les heredocs */
int	prepare_all_heredocs(t_ast *n, t_data *d)
{
	if (!n)
		return (0);
	if (n->type == NODE_CMD)
	{
		if (run_heredocs_for_redirs(n->redirs, d) != 0)
			return (1);
	}
	if (prepare_all_heredocs(n->left, d) != 0)
		return (1);
	if (prepare_all_heredocs(n->right, d) != 0)
		return (1);
	if (prepare_all_heredocs(n->child, d) != 0)
		return (1);
	return (0);
}
