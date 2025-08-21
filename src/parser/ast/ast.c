#include "minishell.h"

t_ast *parse_sequence(t_token **tokens);
t_ast *parse_and_or(t_token **tokens);
t_ast *parse_pipeline(t_token **tokens);
t_ast *parse_command(t_token **tokens);
t_ast *parse_group(t_token **tokens);

static int count_args(t_token *tok)
{
    int count = 0;

    while (tok && tok->type == T_WORD)
    {
        count++;
        tok = tok->next;
    }
    return (count);
}

// Construit argv[] à partir des tokens
char **build_argv(t_token *tok)
{
    int     count;
    char  **argv;
    int     i;

    count = count_args(tok);
    argv = malloc(sizeof(char *) * (count + 1));
    if (!argv)
        return (NULL);
    i = 0;
    while (tok && tok->type == T_WORD)
    {
        argv[i] = ft_strdup(tok->value); // ✅ on duplique pour éviter des free doubles
        if (!argv[i])
        {
            free_tab(argv);
            return (NULL);
        }
        i++;
        tok = tok->next;
    }
    argv[i] = NULL;
    return (argv);
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
    t_ast   *cmd;
    t_token *tok;
    t_token *start;

    cmd = new_ast_node(NODE_CMD);
    start = *tokens;
    tok = start;
    // 1️⃣ Construire argv une bonne fois
    cmd->argv = build_argv(tok);
    // 2️⃣ Avancer les tokens (on s'arrête quand ce n'est plus du mot/redir)
    while (tok && tok->type != T_AND && tok->type != T_OR
           && tok->type != T_PIPE && tok->type != T_SEPARATOR
           && tok->type != T_PAREN_CLOSE)
    {
        if (tok->type == T_WORD)
            tok = tok->next;
        else if (tok->type == T_REDIR_IN || tok->type == T_REDIR_OUT
                 || tok->type == T_APPEND || tok->type == T_HEREDOC)
        {
            tok = parse_redirection(cmd, tok);
            if (!tok)
                return NULL; // erreur déjà affichée
        }
        else
            tok = tok->next;
    }
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
