/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:06:58 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 18:09:20 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	wait_status_to_code(int s)
{
	if (WIFEXITED(s))
		return (WEXITSTATUS(s));
	if (WIFSIGNALED(s))
		return (128 + WTERMSIG(s));
	return (1);
}

int	wait_and_get_status(pid_t pid)
{
	int	s;

	if (waitpid(pid, &s, 0) < 0)
	{
		err_sys_label("waitpid");
		return (1);
	}
	return (wait_status_to_code(s));
}
