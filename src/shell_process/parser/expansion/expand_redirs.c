/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redirs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 01:10:19 by sberete           #+#    #+#             */
/*   Updated: 2025/09/19 01:10:38 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* redirections: expand filename in-place si autorisé, puis retirer quotes */
int	expand_redirs_inplace(t_redir *r, t_data *data)
{
	char	*tmp;
	char	*uq;

	while (r)
	{
		if (r->filename && r->filename_can_expand)
		{
			tmp = ms_expand_vars(r->filename, data);
			if (!tmp)
				return (1);
			uq = unquote_all(tmp);
			free(tmp);
			if (!uq)
				return (1);
			free(r->filename);
			r->filename = uq;
		}
		r = r->next;
	}
	return (0);
}
