#include "minishell.h"

static void	print_indent(int depth)
{
	int i = 0;
	while (i < depth)
	{
		printf("  ");
		i++;
	}
}

static void	print_redirs(t_redir *redir, int depth)
{
	while (redir)
	{
		print_indent(depth);
		printf("REDIR: ");
		if (redir->type == REDIR_IN)
			printf("< %s\n", redir->filename);
		else if (redir->type == REDIR_OUT)
			printf("> %s\n", redir->filename);
		else if (redir->type == REDIR_APPEND)
			printf(">> %s\n", redir->filename);
		else if (redir->type == REDIR_HEREDOC)
			printf("<< %s\n", redir->filename);
		redir = redir->next;
	}
}

void	print_ast(t_ast *node, int depth)
{
	int i;

	if (!node)
		return;
	print_indent(depth);
	if (node->type == NODE_CMD)
	{
		printf("CMD: ");
		i = 0;
		if (node->argv)
		{
			while (node->argv[i])
			{
				printf("%s ", node->argv[i]);
				i++;
			}
		}
		printf("\n");
		print_redirs(node->redirs, depth + 1);
	}
	else if (node->type == NODE_PIPE)
		printf("PIPE\n");
	else if (node->type == NODE_AND)
		printf("AND (&&)\n");
	else if (node->type == NODE_OR)
		printf("OR (||)\n");
	else if (node->type == NODE_SEQ)
		printf("SEQ (;)\n");
	else if (node->type == NODE_GROUP)
		printf("GROUP (subshell)\n");

	print_ast(node->left, depth + 1);
	print_ast(node->right, depth + 1);
	print_ast(node->child, depth + 1);
}