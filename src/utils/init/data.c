/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 19:27:46 by sberete           #+#    #+#             */
/*   Updated: 2025/09/15 01:55:42 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_data	data_init(int argc, char **argv, char **envp)
{
	t_data	d;

	(void)argc;
	(void)argv;
	ft_bzero(&d, sizeof(t_data));
	d.env = env_from_envp(envp);
	if (d.env)
		(void)env_inc_shlvl(&d.env);
	d.last_exit = 0;
	return (d);
}
