/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 19:29:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:45:01 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Handler parent au prompt: efface la ligne et passe à la ligne.
   PAS de rl_redisplay, PAS de rl_done -> readline réaffichera un seul prompt. */
/* Parent au prompt: coupe immédiatement la readline courante */
/* ----- Parent pendant un heredoc child: on ignore ^C/^\. */
// static void	parent_sigint(int signo)
// {
// 	(void)signo;
// 	g_exit_status = 130;
// 	write(STDOUT_FILENO, "\n", 1);
// 	rl_replace_line("", 0);
// 	rl_on_new_line();
// 	rl_redisplay();
// }

// /* On ignore Ctrl-\ en parent interactif (pas de "Quit (core dumped)") */
// static void	parent_sigquit(int signo)
// {
// 	(void)signo;
// 	rl_on_new_line();
// 	rl_redisplay();
// }

// /* À appeler avant la boucle readline() et après un exec/wait pour rétablir */
// void	signals_setup_parent(void)
// {
// 	/* SA_RESTART rend readline plus robuste ; ok avec GNU Readline */
// 	set_sigaction(SIGINT, parent_sigint, SA_RESTART);
// 	set_sigaction(SIGQUIT, parent_sigquit, 0);
// }

// /* À appeler juste avant d'entrer en phase d'attente (waitpid) :
//    on ignore SIGINT/SIGQUIT côté parent pendant que les enfants reçoivent DFL. */
// void	signals_setup_parent_execwait(void)
// {
// 	set_signal_ign(SIGINT);
// 	set_signal_ign(SIGQUIT);
// }

/* ───────────────────────── Handlers parent ───────────────────────── */

static void	parent_sigint(int signo)
{
	(void)signo;
	g_exit_status = 130;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/* ───────────────────────── Parent setup ───────────────────────── */

void	signals_setup_parent(void)
{
	struct sigaction	sa;

	/* SIGINT */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = parent_sigint;
	sigaction(SIGINT, &sa, NULL);
	/* SIGQUIT -> complètement ignoré */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN; // direct
	sigaction(SIGQUIT, &sa, NULL);
	signal(SIGPIPE, SIG_IGN);
}

/* ───────────────────────── Child setup ───────────────────────── */

void	signals_setup_child(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}