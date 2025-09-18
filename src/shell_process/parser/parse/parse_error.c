/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 05:58:53 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 06:11:35 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ps_error(char *msg)
{
	ft_putstr_fd("minishell: syntax error: ", STDERR_FILENO);
	if (msg)
		ft_putstr_fd((char *)msg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
	set_exit_status(2);
	return (0);
}

int	ps_eof_quote(char q)
{
	ft_putstr_fd("minishell: syntax error: unexpected EOF ", STDERR_FILENO);
	ft_putstr_fd("while looking for matching `", STDERR_FILENO);
	write(STDERR_FILENO, &q, 1);
	ft_putstr_fd("'\n", STDERR_FILENO);
	set_exit_status(2);
	return (0);
}
