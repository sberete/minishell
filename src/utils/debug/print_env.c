/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 19:25:04 by sberete           #+#    #+#             */
/*   Updated: 2025/09/05 19:25:05 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_env(t_env *lst)
{
	while (lst)
	{
		if (lst->value && *lst->value)
			printf("%s=%s\n", lst->key, lst->value);
		lst = lst->next;
	}
}
