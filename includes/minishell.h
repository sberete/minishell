/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:46:06 by sberete           #+#    #+#             */
/*   Updated: 2025/08/25 21:46:21 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_printf.h"
# include "get_next_line.h"
# include "libft.h"
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

typedef enum e_token_type
{
	T_WORD,        // ex: ls, -la, fichier.txt
	T_REDIR_IN,    // <
	T_REDIR_OUT,   // >
	T_APPEND,      // >>
	T_HEREDOC,     // <<
	T_AND,         // &&
	T_OR,          // ||
	T_PIPE,        // |
	T_SEPARATOR,   // ;
	T_PAREN_OPEN,  // (
	T_PAREN_CLOSE, // )
	T_WILDCARD,    // *
	T_END,         // Fin de commande
	T_UNKNOWN,     // Tout ce qui ne devrait pas être là
}					t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}					t_token;

typedef enum e_node_type
{
	NODE_CMD,   // Commande simple : ls -l
	NODE_PIPE,  // cmd1 | cmd2
	NODE_AND,   // cmd1 && cmd2
	NODE_OR,    // cmd1 || cmd2
	NODE_SEQ,   // cmd1 ; cmd2
	NODE_GROUP, // (cmd1 | cmd2)
}					t_node_type;

typedef enum e_redir_type
{
	REDIR_IN,      // <
	REDIR_OUT,     // >
	REDIR_APPEND,  // >>
	REDIR_HEREDOC, // <<
}					t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char *filename;       // Nom du fichier ou limiter
	struct s_redir *next; // Permet plusieurs redirections
}					t_redir;

typedef struct s_ast
{
	t_node_type type;    // Type du nœud
	char **argv;         // Tableau d'arguments : ["ls", "-l", NULL]
	t_redir *redirs;     // Liste chaînée des redirections
	struct s_ast *left;  // Nœud gauche pour opérateurs binaires
	struct s_ast *right; // Nœud droit pour opérateurs binaires
	struct s_ast *child; // Sous-shell (NODE_GROUP)
}					t_ast;

typedef struct s_exec
{
	int prev_read;   // fd de lecture du pipe précédent (-1 si aucun)
	int pipe_fd[2];  // fd pour le pipe courant [0] = read, [1] = write
	char *path;      // chemin absolu de la commande
	char **argv;     // arguments (venant de l’AST)
	t_redir *redirs; // redirections à appliquer
	pid_t *pids;     // pid du processus (utile pour wait)
	struct s_data	*data;
}					t_exec;

typedef struct s_data
{
	char *line;      // Readline
	t_token *tokens; // Tokenisation
	t_ast *ast;      // AST
	t_exec *exec;    // Exec
	char **env;      // Copie de l'environnement
	int last_exit;   // Code de sortie de la dernière commande
	bool running;    // Booléen pour savoir si on continue la boucle
}					t_data;

/* Tokens */
t_token				*new_token_node(char *value);
bool				add_token_back(t_token **head, t_token *new);
t_token_type		get_token_type(const char *str);
int					is_operator_start(char c);
int					get_operator_len(const char *str);
int					tokenize_line(t_data *data);
char				*extract_token(t_data *data, int *i);
char				*extract_operator(t_data *data, int *i);
char				*extract_simple(t_data *data, int *i);
char				*extract_quoted(t_data *data, int *i);
void				skip_spaces(t_data *data, int *i);
char				*append_token_part(char *token, char *part);

/* Utils */
void				print_tokens(t_token *tokens);
void				free_token_list(t_token **head);
void				free_data(t_data *data);
void				print_syntax_error(char *msg);

/* AST */
t_ast				*new_ast_node(t_node_type type);
void				free_ast(t_ast *node);
t_redir				*new_redir(t_redir_type type, char *filename);
void				add_redir(t_ast *cmd_node, t_redir *redir);
t_ast				*parse_sequence(t_token **tokens);
void				print_ast(t_ast *node, int depth);
t_token				*parse_redirection(t_ast *cmd, t_token *tok);
int					exec_ast(t_ast *node, t_data *data);

#endif
