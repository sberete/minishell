#include "minishell.h"

t_ast *parse_sequence(t_token **tokens);
t_ast *parse_and_or(t_token **tokens);
t_ast *parse_pipeline(t_token **tokens);
t_ast *parse_command(t_token **tokens);
t_ast *parse_group(t_token **tokens);

static int count_cmd_args(t_token *tok)
{
    int count = 0;

    while (tok && tok->type != T_AND && tok->type != T_OR
           && tok->type != T_PIPE && tok->type != T_SEPARATOR
           && tok->type != T_PAREN_CLOSE)
    {
        if (tok->type == T_WORD)
            count++;
        else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
                 || tok->type == T_APPEND || tok->type == T_HEREDOC)
        {
            tok = tok->next; // skip operator
            if (tok && tok->type == T_WORD) // skip filename
                tok = tok->next;
            continue;
        }
        tok = tok->next;
    }
    return count;
}

t_ast *parse_sequence(t_token **tokens)
{
    t_ast *left = parse_and_or(tokens);
    t_token *tok = *tokens;

    while (tok && tok->type == T_SEPARATOR) // ";"
    {
        *tokens = tok->next;
        t_ast *right = parse_and_or(tokens);
        t_ast *seq_node = new_ast_node(NODE_SEQ);
        seq_node->left = left;
        seq_node->right = right;
        left = seq_node;
        tok = *tokens;
    }
    return left;
}

t_ast *parse_group(t_token **tokens)
{
    if (!*tokens || (*tokens)->type != T_PAREN_OPEN)
        return parse_command(tokens);
    *tokens = (*tokens)->next; // skip '('
    t_ast *child = parse_sequence(tokens);
    if (!*tokens || (*tokens)->type != T_PAREN_CLOSE)
    {
        printf("Erreur: parenthèse non fermée\n");
        return NULL;
    }
    *tokens = (*tokens)->next; // skip ')'

    t_ast *group = new_ast_node(NODE_GROUP);
    group->child = child;
    return group;
}

t_ast *parse_command(t_token **tokens)
{
    t_ast   *cmd = new_ast_node(NODE_CMD);
    t_token *tok = *tokens;
    int      argc = count_cmd_args(tok);
    char   **argv = malloc(sizeof(char *) * (argc + 1));
    int      i = 0;

    if (!argv)
        return NULL;

    while (tok && tok->type != T_AND && tok->type != T_OR
           && tok->type != T_PIPE && tok->type != T_SEPARATOR
           && tok->type != T_PAREN_CLOSE)
    {
        if (tok->type == T_WORD)
        {
            argv[i++] = ft_strdup(tok->value);
            tok = tok->next;
        }
        else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
                 || tok->type == T_APPEND || tok->type == T_HEREDOC)
        {
            tok = parse_redirection(cmd, tok);
            if (!tok)
            {
                free_tab(argv);
                return NULL;
            }
        }
        else
            tok = tok->next;
    }
    argv[i] = NULL;
    cmd->argv = argv;
    *tokens = tok;
    return cmd;
}

t_ast *parse_pipeline(t_token **tokens)
{
    t_ast *left = parse_group(tokens);
    t_token *tok = *tokens;

    while (tok && tok->type == T_PIPE)
    {
        *tokens = tok->next;
        t_ast *right = parse_group(tokens);
        t_ast *pipe_node = new_ast_node(NODE_PIPE);
        pipe_node->left = left;
        pipe_node->right = right;
        left = pipe_node;
        tok = *tokens;
    }
    return left;
}

t_ast *parse_and_or(t_token **tokens)
{
    t_ast *left = parse_pipeline(tokens);
    t_token *tok = *tokens;

    while (tok && (tok->type == T_AND || tok->type == T_OR))
    {
        t_node_type type = (tok->type == T_AND) ? NODE_AND : NODE_OR;
        *tokens = tok->next;
        t_ast *right = parse_pipeline(tokens);
        t_ast *node = new_ast_node(type);
        node->left = left;
        node->right = right;
        left = node;
        tok = *tokens;
    }
    return left;
}
