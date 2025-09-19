/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_argv.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:07:56 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:08:30 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Compat si du code appelle encore l’ancien nom */
char	**expand_argv_dup(t_ast *cmd, t_data *data)
{
	return (expand_argv_full(cmd, data));
}
