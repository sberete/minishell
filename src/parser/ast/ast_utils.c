#include "minishell.h"

t_ast *new_ast_node(t_node_type type)
{
    t_ast *node = malloc(sizeof(t_ast));
    if (!node)
        return NULL;
    node->type = type;
    node->argv = NULL;
    node->redirs = NULL;
    node->left = NULL;
    node->right = NULL;
    node->child = NULL;
    return node;
}

t_redir *new_redir(t_redir_type type, char *filename)
{
    t_redir *redir = malloc(sizeof(t_redir));
    if (!redir)
        return NULL;
    redir->type = type;
    redir->filename = filename;
    redir->next = NULL;
    return redir;
}

void add_redir(t_ast *cmd_node, t_redir *redir)
{
    if (!cmd_node->redirs)
        cmd_node->redirs = redir;
    else
    {
        t_redir *tmp = cmd_node->redirs;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = redir;
    }
}
