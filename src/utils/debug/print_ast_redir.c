/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast_redir.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 21:55:40 by sberete           #+#    #+#             */
/*   Updated: 2025/09/20 22:15:48 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*redir_name(t_redir_type t)
{
	if (t == REDIR_IN)
		return ("<");
	if (t == REDIR_OUT)
		return (">");
	if (t == REDIR_APPEND)
		return (">>");
	if (t == REDIR_HEREDOC)
		return ("<<");
	return ("?");
}

void	print_redirs(t_redir *r, int depth)
{
	char	*fname;
	char	*delim;
	char	*fexp;
	char	*dexp;

	while (r)
	{
		fname = "(null)";
		delim = "(null)";
		fexp = "false";
		dexp = "false";
		if (r->filename)
			fname = r->filename;
		if (r->delim)
			delim = r->delim;
		if (r->filename_can_expand)
			fexp = "true";
		if (r->delim_can_expand)
			dexp = "true";
		print_indent(depth);
		printf("  REDIR: %-2s  filename=%s  delim=%s  fexp=%s  dexp=%s\n",
			redir_name(r->type), fname, delim, fexp, dexp);
		r = r->next;
	}
}
