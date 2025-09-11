/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sxrimu <sxrimu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:46:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/10 19:45:32 by sxrimu           ###   ########.fr       */
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
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#include <errno.h>
extern volatile sig_atomic_t	g_exit_status;

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
}								t_token_type;

typedef struct s_token
{
	char						*value;
	t_token_type				type;
	bool							can_expand;
	struct s_token				*next;
}								t_token;

typedef enum e_node_type
{
	NODE_CMD,   // Commande simple : ls -l
	NODE_PIPE,  // cmd1 | cmd2
	NODE_AND,   // cmd1 && cmd2
	NODE_OR,    // cmd1 || cmd2
	NODE_SEQ,   // cmd1 ; cmd2
	NODE_GROUP, // (cmd1 | cmd2)
}								t_node_type;

typedef enum e_redir_type
{
	REDIR_IN,      // <
	REDIR_OUT,     // >
	REDIR_APPEND,  // >>
	REDIR_HEREDOC, // <<
}								t_redir_type;

typedef struct s_redir
{
	t_redir_type				type;
	char *filename; // Nom du fichier ou limiter
	char						*delim;
	int							fd;
	bool filename_can_expand;
	bool delim_can_expand;
	struct s_redir *next; // Permet plusieurs redirections
}								t_redir;

typedef struct s_ast
{
	t_node_type type;    // Type du nœud
	char **argv;         // Tableau d'arguments : ["ls", "-l", NULL]
	bool *argv_can_expand;
	t_redir *redirs;     // Liste chaînée des redirections
	struct s_ast *left;  // Nœud gauche pour opérateurs binaires
	struct s_ast *right; // Nœud droit pour opérateurs binaires
	struct s_ast *child; // Sous-shell (NODE_GROUP)
}								t_ast;

typedef struct s_exec
{
	int prev_read;   // fd de lecture du pipe précédent (-1 si aucun)
	int pipe_fd[2];  // fd pour le pipe courant [0] = read, [1] = write
	char *path;      // chemin absolu de la commande
	char **argv;     // arguments (venant de l’AST)
	t_redir *redirs; // redirections à appliquer
	pid_t *pids;     // pid du processus (utile pour wait)
	struct s_data				*data;
}								t_exec;

typedef struct s_env
{
	char *key;   // ex: "PATH"
	char *value; // ex: "/usr/bin:/bin"
	struct s_env				*next;
}								t_env;

typedef struct s_data
{
	char *line;       // Readline
	t_token *tokens;  // Tokenisation
	t_ast *ast;       // AST
	t_exec *exec;     // Exec
	t_env *env;       // Copie de l'environnement
	int last_exit;    // Code de sortie de la dernière commande
	int input_status; // 1 si readline a renvoyé une ligne, 0 si Ctrl-D
}								t_data;

/* Tokens */
t_token							*new_token_node(char *value);
bool							add_token_back(t_token **head, t_token *new);
t_token_type					get_token_type(const char *str);
int								is_operator_start(char c);
int								get_operator_len(const char *str);
char							*extract_word(t_data *data, int *i);
int								tokenize_line(t_data *data);
char							*extract_token(t_data *data, int *i);
char							*extract_operator(t_data *data, int *i);
void							skip_spaces(t_data *data, int *i);
char							*append_token_part(char *token, char *part);

/* Utils */
void							print_tokens(t_token *tokens);
void							free_token_list(t_token **head);
void							free_data(t_data *data);
void							print_syntax_error(char *msg);

/* AST */
t_ast							*new_ast_node(t_node_type type);
void							free_ast(t_ast *node);
t_redir *new_redir(t_redir_type type,
                   char *filename, char *delim,
                   bool filename_can_expand, bool delim_can_expand);

void							add_redir(t_ast *cmd_node, t_redir *redir);
t_ast							*parse_sequence(t_token **tokens);
void							print_ast(t_ast *node, int depth);
t_token							*parse_redirection(t_ast *cmd, t_token *tok);
int								exec_ast(t_ast *node, t_data *data);
int							apply_redirs(t_redir *redirs);
char							**get_env_path(char **env);
char							*resolve_path(char *cmd, char **env);
t_env							*env_from_environ(char **environ);
t_data							data_init(int argc, char **argv, char **env);
int								id_is_valid(const char *s);
void							free_data_tmp(t_data *data);

int								split_key_value(const char *s, char **k,
									char **v);

int								split_key_append(const char *s, char **k,
									char **v, int *is_append);

char							*ms_getenv(t_env *lst, const char *key);

int								ms_setenv(t_env **lst, const char *key,
									const char *value, int overwrite);

int								ms_setenv_eq(t_env **lst, const char *assign);

int								ms_setenv_append(t_env **lst,
									const char *assign);
t_env							*env_new(const char *key, const char *value);
void							env_add_back(t_env **lst, t_env *node);
size_t							env_size(t_env *lst);

t_env							*env_find(t_env *lst, const char *key);

int								env_remove(t_env **lst, const char *key);
void							env_clear(t_env **lst);
int								shell_process(t_data *data);
int								read_input(t_data *data);

void							signals_setup_parent(void);
void							parent_signals_init(void);
void							signals_setup_child(void);
void							signals_setup_heredoc_parent(void);
void							signals_setup_heredoc_child(void);
void							sync_exit_status(t_data *data);
/* ========= EXEC ========= */
int     exec_ast(t_ast *node, t_data *data);

/* Builtins (détection + exécution) */
int     is_builtin(const char *name);
int     run_builtin(char **argv, t_data *data);
int     run_builtin_parent(t_ast *cmd, t_data *data);

/* Externes */
int     run_external(char **argv, t_data *data);

/* Redirections / heredoc */
int     apply_redirs(t_redir *redirs);
int     run_heredocs_for_redirs(t_redir *redirs, t_data *data); /* stub pour l’instant */

/* Env list <-> envp */
char  **env_list_to_envp(t_env *lst);
void    free_envp(char **envp);

/* PATH */
char   *find_cmd_path(char *cmd, t_env *envlst);

/* Wait status helpers */
int     normalize_wait_status(int wstatus);
int     wait_and_get_status(pid_t pid);

/* Signals enfant */
void    signals_setup_parent(void);         /* parent interactif (readline + rl_replace_line) */
void    signals_setup_parent_execwait(void);/* parent pendant un wait: ignore INT/QUIT */
void    signals_setup_child(void);          /* enfants exec/pipes/subshell */
void    signals_setup_heredoc_parent(void); /* parent pendant heredoc */
void    signals_setup_heredoc_child(void);  /* child heredoc */
int	set_sigaction(int signum, void (*handler)(int), int flags);
int	set_signal_ign(int signum);
int	set_signal_dfl(int signum);
char *hd_strip_outer_quotes(const char *s);
char *heredoc_expand_line(const char *line, t_data *data);
/* Heredoc */
int   run_heredocs_for_redirs(t_redir *redirs, t_data *data);

/* Signals (déjà posés) */
void  signals_setup_parent(void);
void  signals_setup_parent_execwait(void);
void  signals_setup_child(void);
void  signals_setup_heredoc_parent(void);
void  signals_setup_heredoc_child(void);
int ft_echo(char **argv, t_data *data);
int ft_cd(char **argv, t_data *data);
int ft_pwd(char **argv, t_data *data);
int ft_export(char **argv, t_data *data);
int ft_unset(char **argv, t_data *data);
int ft_env(char **argv, t_data *data);
int ft_exit(char **argv, t_data *data);
void  child_free_and_exit(t_data *data, int status);
char **env_list_to_envp(t_env *lst);
void  free_envp(char **envp);
char *find_cmd_path(char *cmd, t_env *envlst);
int   wait_and_update_exit(pid_t pid);
int   save_stdio(int *saved_in, int *saved_out);
void  restore_stdio(int saved_in, int saved_out);
void	update_exit_from_wait(int status);

#endif
