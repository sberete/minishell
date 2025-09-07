/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_signal.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 17:13:56 by sberete           #+#    #+#             */
/*   Updated: 2025/09/07 17:56:09 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ───────────────────────── Heredoc setup ─────────────────────────
   Recommandation : faire le heredoc dans un CHILD dédié.
   - Parent : peut laisser SIGINT tel quel, ou l'ignorer pendant l'attente.
   - Child heredoc : défaut pour que ^C interrompe la lecture (exit 130). */

void	signals_setup_heredoc_parent(void)
{
	struct sigaction	sa;

	/* Ici, soit tu ignores ^C pendant que le child heredoc tourne,
		soit tu le traites (au choix). On choisit de l'ignorer pour
		ne pas casser le shell si l'utilisateur spam ^C pendant l'attente. */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	/* On ignore aussi SIGQUIT par cohérence. */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

void	signals_setup_heredoc_child(void)
{
	struct sigaction	sa;

	/* défaut : ^C/^\\ tuent le child heredoc (le parent verra 130/131) */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
