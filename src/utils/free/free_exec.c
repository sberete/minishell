/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:00:36 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 00:54:27 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	child_free_and_exit(char **argv, char **envp, char *path, int code)
{
	if (argv)
		free_tab(argv);
	if (envp)
		free_tab(envp);
	if (path)
		free(path);
	exit(code);
}
