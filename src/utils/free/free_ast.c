#include "minishell.h"

static void	free_redirs(t_redir *redir)
{
	t_redir *tmp;

	while (redir)
	{
		tmp = redir->next;
		free(redir->filename);
		free(redir);
		redir = tmp;
	}
}

void	free_ast(t_ast *node)
{
    int i;

	if (!node)
    return;
    
	free_ast(node->left);
	free_ast(node->right);
	free_ast(node->child);
	if (node->argv)
	{
        i = 0;
		while (node->argv[i])
			free(node->argv[i++]);
		free(node->argv);
	}
	free_redirs(node->redirs);
	free(node);
}