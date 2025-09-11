/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 19:53:11 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 18:23:30 by sxrimu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_exit_status = 0;

int main(int argc, char **argv, char **env)
{
    t_data data;

    rl_catch_signals = 0;      /* NOS handlers, pas ceux de readline */
    data = data_init(argc, argv, env);
    signals_setup_parent();

    while (1)
    {
        int rc = read_input(&data);
        // sync_exit_status(&data); /* suit g_exit_status (130 après ^C au prompt) */

        if (rc == 0) { free_data(&data); break; }     /* EOF */
        if (rc == 2) { free_data_tmp(&data); continue; } /* vide -> on zappe */

        if (shell_process(&data) == 1)
        {
            free_data_tmp(&data);
            continue;
        }
        free_data_tmp(&data);
    }
    return data.last_exit;
}
/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/
