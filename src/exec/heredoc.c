/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you <you@student.42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 00:00:00 by you               #+#    #+#             */
/*   Updated: 2025/09/01 00:00:00 by you              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
**  FICHIER AUTONOME (tout-en-un) :
**  - A inclure tel quel dans le projet.
**  - Dépend UNIQUEMENT de "minishell.h" pour t_ast / t_redir / enums.
**  - Ne dépend pas de libft : utilitaires locaux (ms_strlen/ms_strcmp).
**  - Ne touche pas à une globale (pas de g_exit_status ici).
**  - RENSEIGNE via *interrupted si Ctrl-C pendant un heredoc (à toi de mettre $?=130).
**
**  INTÉGRATION MINIMALE CÔTÉ APPELANT :
**    int interrupted = 0;
**    if (prepare_heredocs(ast, &interrupted) == -1) {  erreur sys  }
**    if (interrupted) {
**        // Ctrl-C durant heredoc : annule l'exec de la ligne
**        // (optionnel) set status = 130 ici
**        close_heredoc_fds(ast);
**        return ;
**    }
**    // exec pipeline...
**    close_heredoc_fds(ast);
**
**  DANS L'ENFANT (avant execve), pour appliquer la redirection :
**    if (r->type == REDIR_HEREDOC) { dup2(r->fd, STDIN_FILENO); }
*/

#include "minishell.h"

/* ───────────────────────────── Gestion signaux ──────────────────────────── */

static void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
}

static void	set_parent_signals_during_heredoc(void)
{
	/* Pendant la saisie heredoc (dans l'enfant), on protège le parent. */
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

static void	restore_interactive_signals(void)
{
	/* Version simple : parent ignore SIGQUIT et laisse SIGINT
	   (ton handler readline pourra être (ré)installé ailleurs). */
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

/* ──────────────────────────── I/O vers le pipe ──────────────────────────── */

/* Écrit s + '\n' dans fd; retourne -1 en cas d'erreur d'écriture. */
static int	write_line_nl(int fd, const char *s)
{
	size_t	len;
	ssize_t	w;

	if (!s)
		return (0);
	len = ms_strlen(s);
	w = write(fd, s, len);
	if (w < 0 || (size_t)w != len)
		return (-1);
	if (write(fd, "\n", 1) != 1)
		return (-1);
	return (0);
}

/* ───────────────────── Création d'un FD de heredoc ──────────────────────── */
/*
** Retourne 0 si OK, -1 si erreur, et via *interrupted signale un Ctrl-C.
** - limiter : le mot de fin (EOF, STOP, ...)
** - *out_read_fd : renvoie l'extrémité LECTURE remplie par l'enfant
** - *interrupted : mis à 1 si l'enfant a été tué par SIGINT
*/
static int	create_heredoc_fd(const char *limiter, int *out_read_fd,
	int *interrupted)
{
	int		pp[2];
	pid_t	pid;
	int		status;
	char	*line;

	*out_read_fd = -1;
	if (pipe(pp) == -1)
		return (-1);
	set_parent_signals_during_heredoc();
	pid = fork();
	if (pid < 0)
	{
		close(pp[0]);
		close(pp[1]);
		return (-1);
	}
	if (pid == 0)
	{
		/* ── ENFANT : lit des lignes et remplit pp[1] ── */
		set_child_signals();
		close(pp[0]);
		while (1)
		{
			line = readline("> ");
			if (!line) /* Ctrl-D */
				break;
			if (ft_strcmp(line, limiter) == 0)
			{
				free(line);
				break;
			}
			if (write_line_nl(pp[1], line) == -1)
			{
				free(line);
				_exit(1);
			}
			free(line);
		}
		close(pp[1]);
		_exit(0);
	}
	/* ── PARENT ── */
	close(pp[1]);
	if (waitpid(pid, &status, 0) < 0)
	{
		close(pp[0]);
		restore_interactive_signals();
		return (-1);
	}
	restore_interactive_signals();
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		*interrupted = 1;
		close(pp[0]);
		return (0);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		close(pp[0]);
		return (-1);
	}
	*out_read_fd = pp[0];
	return (0);
}

/* ──────────────────────────── Parcours de l'AST ─────────────────────────── */

static int	prepare_heredocs_cmd(t_ast *cmd, int *interrupted)
{
	t_redir	*r;
	int		fd;

	r = cmd->redirs;
	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			fd = -1;
			if (create_heredoc_fd(r->filename, &fd, interrupted) == -1)
				return (-1);
			if (*interrupted)
				return (0);
			r->fd = fd;
		}
		r = r->next;
	}
	return (0);
}

/*
** Prépare tous les heredocs de l'AST avant exécution.
** - Retourne -1 si erreur système (pipe/fork/write/wait).
** - Met *interrupted = 1 si un heredoc a été interrompu (Ctrl-C).
** - Retourne 0 sinon.
*/
int	prepare_heredocs(t_ast *root, int *interrupted)
{
	int	ret;

	if (!root || !interrupted)
		return (0);
	*interrupted = 0;
	if (root->type == NODE_CMD)
		return (prepare_heredocs_cmd(root, interrupted));
	ret = 0;
	if (root->left)
	{
		ret = prepare_heredocs(root->left, interrupted);
		if (ret == -1 || *interrupted)
			return (ret);
	}
	if (root->right)
	{
		ret = prepare_heredocs(root->right, interrupted);
		if (ret == -1 || *interrupted)
			return (ret);
	}
	if (root->child)
	{
		ret = prepare_heredocs(root->child, interrupted);
		if (ret == -1 || *interrupted)
			return (ret);
	}
	return (0);
}

/* ─────────────────── Fermeture propre des FDs heredoc ───────────────────── */

static void	close_heredoc_fds_cmd(t_ast *cmd)
{
	t_redir	*r;

	r = cmd->redirs;
	while (r)
	{
		if (r->type == REDIR_HEREDOC && r->fd >= 0)
		{
			close(r->fd);
			r->fd = -1;
		}
		r = r->next;
	}
}

/*
** À appeler après l'exécution (ou si interrompu) pour fermer
** tous les FDs de type HEREDOC attachés à l'AST.
*/
void	close_heredoc_fds(t_ast *root)
{
	if (!root)
		return ;
	if (root->type == NODE_CMD)
		close_heredoc_fds_cmd(root);
	if (root->left)
		close_heredoc_fds(root->left);
	if (root->right)
		close_heredoc_fds(root->right);
	if (root->child)
		close_heredoc_fds(root->child);
}
