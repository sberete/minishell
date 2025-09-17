#include "minishell.h"
#include <stdio.h>              /* fprintf, perror */
#include <unistd.h>             /* pipe, fork, dup2, close, write */
#include <sys/wait.h>           /* waitpid */
#include <readline/readline.h>  /* readline */

/* --------- utils chaînes (tes helpers) ---------- */

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

/* expansion des variables (si activée) */
char	*heredoc_expand_line(char *line, t_data *data)
{
	return (ms_expand_vars(line, data));
}

/* y a-t-il des quotes dans le délimiteur ? (si oui: NO EXPANSION, POSIX) */
static int	heredoc_delim_is_quoted(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'' || s[i] == '\"')
			return (1);
		i++;
	}
	return (0);
}

/* write(fd, buf, len) jusqu’au bout ; retourne 0 ou -1 et laisse errno */
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

/* boucle de saisie côté enfant */
static int	heredoc_loop(int wfd, const char *delim, int do_expand, t_data *d)
{
	char	*line;
	char	*exp;

	while (1)
	{
		line = readline("> ");
		if (line == NULL) /* EOF: termine comme si délimiteur */
			return (0);
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			return (0);
		}
		if (do_expand)
			exp = heredoc_expand_line(line, d);
		else
			exp = ft_strdup(line);
		free(line);
		if (!exp)
		{
			fprintf(stderr, "minishell: heredoc: allocation failed\n");
			return (1);
		}
		if (write_all(wfd, exp, ft_strlen(exp)) < 0
			|| write_all(wfd, "\n", 1) < 0)
		{
			fprintf(stderr, "minishell: heredoc write: ");
			perror(NULL);
			free(exp);
			return (1);
		}
		free(exp);
	}
}

/* enfant: prépare plain delim (non-quoté), boucle, écrit dans wfd */
static int	run_one_heredoc_child(int wfd, t_redir *r, t_data *d)
{
	char	*plain;
	int		do_expand;
	int		rc;

	signals_setup_heredoc_child(); /* doit gérer ^C → SIGINT */
	if (!r || !r->filename)
		return (1);
	do_expand = !heredoc_delim_is_quoted(r->filename);
	plain = hd_strip_outer_quotes(r->filename);
	if (!plain)
	{
		fprintf(stderr, "minishell: heredoc: allocation failed\n");
		return (1);
	}
	rc = heredoc_loop(wfd, plain, do_expand, d);
	free(plain);
	return (rc);
}

/* parent: crée pipe, fork; stocke le fd de lecture dans r->fd, gère ^C */
static int	run_one_heredoc(t_redir *r, t_data *d)
{
	int		pfd[2];
	pid_t	pid;
	int		s;

	if (!r || r->type != REDIR_HEREDOC)
		return (0);
	if (pipe(pfd) != 0)
	{
		fprintf(stderr, "minishell: pipe: ");
		perror(NULL);
		return (1);
	}
	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "minishell: fork: ");
		perror(NULL);
		close(pfd[0]);
		close(pfd[1]);
		return (1);
	}
	if (pid == 0)
	{
		/* enfant: écrit -> pfd[1] ; fermer lecture */
		close(pfd[0]);
		if (run_one_heredoc_child(pfd[1], r, d) != 0)
			_exit(1);
		close(pfd[1]);
		_exit(0);
	}
	/* parent */
	close(pfd[1]);
	signals_setup_heredoc_parent();
	if (waitpid(pid, &s, 0) < 0)
	{
		fprintf(stderr, "minishell: waitpid: ");
		perror(NULL);
		close(pfd[0]);
		return (1);
	}
	if (WIFSIGNALED(s) && WTERMSIG(s) == SIGINT)
	{
		/* ^C pendant heredoc: abort; code 130 */
		close(pfd[0]);
		d->last_exit = 130;
		return (130);
	}
	if (!WIFEXITED(s) || WEXITSTATUS(s) != 0)
	{
		close(pfd[0]);
		return (1);
	}
	r->fd = pfd[0]; /* apply_redirs() fera dup2 + close */
	return (0);
}

/* exécute pour toutes les redirections d’un même nœud commande */
int	run_heredocs_for_redirs(t_redir *r, t_data *d)
{
	int	rc;

	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			rc = run_one_heredoc(r, d);
			if (rc != 0)
				return (rc == 130 ? 130 : 1);
		}
		r = r->next;
	}
	return (0);
}

/* parcours l’AST et prépare tous les heredocs AVANT exécution */
int	prepare_all_heredocs(t_ast *n, t_data *d)
{
	int	rc;

	if (!n)
		return (0);
	if (n->type == NODE_CMD)
	{
		rc = run_heredocs_for_redirs(n->redirs, d);
		if (rc != 0)
			return (rc);
	}
	rc = prepare_all_heredocs(n->left, d);
	if (rc != 0)
		return (rc);
	rc = prepare_all_heredocs(n->right, d);
	if (rc != 0)
		return (rc);
	rc = prepare_all_heredocs(n->child, d);
	if (rc != 0)
		return (rc);
	return (0);
}
