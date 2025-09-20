/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 22:41:13 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:47:34 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	pipeline_parent(pid_t lpid, pid_t rpid, int p[2], t_data *data)
{
	int	st;
	int	rc;

	st = 0;
	rc = 0;
	xclose(&p[0]);
	xclose(&p[1]);
	if (lpid > 0)
	{
		if (waitpid(lpid, NULL, 0) < 0)
			rc |= err_sys_ctx("waitpid");
	}
	if (rpid > 0)
	{
		if (waitpid(rpid, &st, 0) < 0)
			rc |= err_sys_ctx("waitpid");
		else
			data->last_exit = wait_status_to_code(st);
	}
	if (rc)
		return (1);
	return (data->last_exit);
}

int	exec_pipeline_node(t_ast *n, t_data *data)
{
	int		p[2];
	pid_t	lpid;
	pid_t	rpid;

	if (!n || !n->left || !n->right)
		return (1);
	if (pipe(p) < 0)
		return (err_sys_ctx("pipe"));
	lpid = spawn_left(n->left, data, p);
	xclose(&p[1]);
	if (lpid < 0)
	{
		xclose(&p[0]);
		return (1);
	}
	rpid = spawn_right(n->right, data, p);
	xclose(&p[0]);
	{
		waitpid(lpid, NULL, 0);
		return (1);
	}
	return (pipeline_parent(lpid, rpid, p, data));
}
