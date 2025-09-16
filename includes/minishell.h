/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:46:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/17 01:24:20 by sberete          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_printf.h"
# include "get_next_line.h"
# include "libft.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

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
	bool						can_expand;
	struct s_token				*next;
}								t_token;

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
	bool						filename_can_expand;
	bool						delim_can_expand;
	struct s_redir *next; // Permet plusieurs redirections
}								t_redir;

typedef enum e_node_type
{
	NODE_CMD,   // Commande simple : ls -l
	NODE_PIPE,  // cmd1 | cmd2
	NODE_AND,   // cmd1 && cmd2
	NODE_OR,    // cmd1 || cmd2
	NODE_SEQ,   // cmd1 ; cmd2
	NODE_GROUP, // (cmd1 | cmd2)
}								t_node_type;

typedef struct s_ast
{
	t_node_type type; // Type du nœud
	char **argv;      // Tableau d'arguments : ["ls", "-l", NULL]
	bool						*argv_can_expand;
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

/* ========= LEXER ========= */
t_token							*new_token_node(char *value);
bool							add_token_back(t_token **head, t_token *newtok);
t_token_type					get_token_type(const char *str);
int								is_operator_start(char c);
int								get_operator_len(const char *str);
char							*extract_word(t_data *data, int *i);
char							*extract_operator(t_data *data, int *i);
char							*extract_token(t_data *data, int *i);
int								tokenize_line(t_data *data);
void							skip_spaces(t_data *data, int *i);
char							*append_token_part(char *token, char *part);

/* ========= UTILS (generic) ========= */
void							print_tokens(t_token *tokens);
void							free_token_list(t_token **head);
void							print_syntax_error(char *msg);
void							free_data(t_data *data);
void							free_data_tmp(t_data *data);
int								read_input(t_data *data);

/* ========= AST ========= */
t_ast							*new_ast_node(t_node_type type);
bool							add_arg(t_ast *cmd, char *s, bool can_expand);

t_redir							*new_redir(t_redir_type type, char *text,
									bool can_exp);

void							add_redir(t_ast *cmd_node, t_redir *redir);
t_ast							*parse_sequence(t_token **tokens);
void							print_ast(t_ast *node, int depth);
void							free_ast(t_ast *node);

/* ========= EXEC / HEREDOC ========= */
int								exec_ast(t_ast *node, t_data *data);

/* heredoc (préparation en amont) */
int								prepare_one_heredoc(t_redir *r, t_data *data);
int								prepare_all_heredocs(t_ast *node, t_data *data);

/* redirections (à appliquer dans le child juste avant exec) */
/* stdio save/restore + dup util */
int								save_stdio(int *saved_in, int *saved_out);
void							restore_stdio(int saved_in, int saved_out);
int								dup2_and_close(int from, int to);

/* ========= BUILT-IN UTILS ========= */
int								str_eq(char *a, char *b);
int								builtin_argc(char **argv);
void							builtin_err(char *cmd, char *msg);
void							builtin_err_arg(char *cmd, char *arg,
									char *msg);
int								builtin_is_numeric(char *s);

/* ========= BUILT-INS ========= */
int								is_builtin(char *name);
int								exec_builtin(t_data *data, t_ast *cmd);
int								builtin_echo(t_data *d, char **av);
int								builtin_cd(t_data *d, char **av);
int								builtin_pwd(t_data *d, char **av);
int								builtin_export(t_data *d, char **av);
int								builtin_unset(t_data *d, char **av);
int								builtin_env(t_data *d, char **av);
int								builtin_exit(t_data *d, char **av);

/* ========= ENV: core list ========= */
t_env							*env_new(char *key, char *value);
void							env_add_back(t_env **lst, t_env *node);
t_env							*env_find(t_env *lst, char *key);
void							free_env(t_env **lst);

/* ========= ENV: bootstrap (envp <-> list) ========= */
t_env							*env_from_envp(char **envp);
char							**env_list_to_envp(t_env *lst);

/* ========= ENV: utils (validation, get/set/append, parse) ========= */
int								id_is_valid(char *s);
int								parse_assignment(char *s, char **key,
									char **val, int *append);
char							*env_get(t_env *lst, char *key);
int								env_set(t_env **lst, char *key, char *value,
									int overwrite);
int								env_append(t_env **lst, char *key,
									char *suffix);

/* ========= ENV: init/print ========= */
int								env_inc_shlvl(t_env **lst);
void							export_print_sorted(t_env *lst);

/* ========= SIGNALS ========= */
void							signals_setup_parent(void);
void							signals_setup_parent_execwait(void);
void							signals_setup_child(void);
void							signals_setup_heredoc_parent(void);
void							signals_setup_heredoc_child(void);
void							sync_exit_status(t_data *data);
int								set_sigaction(int signum, void (*handler)(int),
									int flags);
int								set_signal_ign(int signum);
int								set_signal_dfl(int signum);

/* ========= MISC ========= */
t_data							data_init(int argc, char **argv, char **envp);
int								shell_process(t_data *data);
int								unset_is_valid_name(char *s);
void							print_env(t_env *lst);
int								unset_remove_key(t_env **env, char *name);
int								export_set_assignment(t_env **lst,
									char *assign);
/* expansion — commun */
void							quote_step(char c, int *in_s, int *in_d);
char							*str_append_free(char *dst, const char *src);
char							*substr_dup(const char *s, size_t pos,
									size_t len);

/* expansion — variables */
int								var_name_start(char c);
int								var_name_char(char c);
size_t							var_name_len(const char *s, size_t pos);
char							*var_value_dup(const char *name, t_data *data);
char							*ms_expand_vars(const char *s, t_data *data);
char							**expand_argv_vars(char **argv,
									bool *can_expand, t_data *data);

/* expansion — glob */
int								glob_match_name(const char *name,
									const char *pat);
void							str_array_insertion_sort(char **arr, size_t n);
char							**expand_argv_glob(char **argv);
/* redirs */
t_redir							*new_redir(t_redir_type type, char *text,
									bool can_exp);

/* parser redirection: consomme op + WORD, avance *tokp, 0=OK, 1=err */
int								parse_redirection(t_ast *cmd, t_token **tokp);
char	*hd_strip_outer_quotes(char *s);
char	*heredoc_expand_line(char *line, t_data *data);
int	save_stdio(int *saved_in, int *saved_out);
void	restore_stdio(int saved_in, int saved_out);
int	wait_and_get_status(pid_t pid);
int	run_subshell(t_ast *sub, t_data *data);
/* ========= EXEC / REDIRS / PATH ========= */
/* redirections complètes (appliquées dans le child) */
int     apply_redirs(t_redir *rlist);  /* tu l’as déjà, garde ce nom */

/* redirect stdio (before exec), simple helper */
int     redirect_io(int in_fd, int out_fd);
int	wait_status_to_code(int s);

/* recherche du binaire dans PATH (liste chaînée env) */
char	*find_cmd_path(char *name, t_env *env);


/* ========= EXPANSION (argv + redirs) ========= */
int     expand_redirs_inplace(t_redir *rlist, t_data *data);

/* renvoie un **NOUVEAU** argv expansé (vars + glob), NULL-terminé */
char    **expand_argv_full(t_ast *cmd, t_data *data);

/* ========= UTILS tableaus ========= */
char	**expand_argv_glob(char **argv);
int	glob_match_name(const char *name, const char *pat);
void	str_array_insertion_sort(char **arr, size_t n);
char	*ms_expand_vars(const char *s, t_data *data);
char	**expand_argv_vars(char **argv, bool *can_expand, t_data *data);
int	var_name_start(char c);
int	var_name_char(char c);
size_t	var_name_len(const char *s, size_t pos);
char	*var_value_dup(const char *name, t_data *data);
char	*env_get(t_env *lst, char *key);
void	quote_step(char c, int *in_s, int *in_d);
char	*str_append_free(char *dst, const char *src);
char	*substr_dup(const char *s, size_t pos, size_t len);
int expand_redirs_inplace(t_redir *r, t_data *data);
char **expand_argv_full(t_ast *cmd, t_data *data);
char **expand_argv_dup(t_ast *cmd, t_data *data); /* si utilisé ailleurs */
int var_name_start(char c);
size_t var_name_extract(const char *s, size_t i, char **name_out);
char *var_expand_value(const char *name, t_data *data);
int	wait_and_get_status(pid_t pid);
int run_builtin_argv(t_data *data, char **av);
int shell_process(t_data *data);
t_ast  *parse_entry(t_data *data);
void	set_exit_status(int code);
void	print_indent(int depth);
void	print_redirs(t_redir *r, int depth);
size_t	env_list_size(t_env *lst);

#endif /* MINISHELL_H */