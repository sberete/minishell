/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sberete <sberete@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 21:46:06 by sberete           #+#    #+#             */
/*   Updated: 2025/09/18 06:07:59 by sberete          ###   ########.fr       */
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

/* ============================== TOKEN TYPES ============================== */

typedef enum e_token_type
{
	T_WORD,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC,
	T_AND,
	T_OR,
	T_PIPE,
	T_SEPARATOR,
	T_PAREN_OPEN,
	T_PAREN_CLOSE,
	T_WILDCARD,
	T_END,
	T_UNKNOWN
}								t_token_type;

typedef struct s_token
{
	char						*value;
	t_token_type				type;
	bool						can_expand;
	struct s_token				*next;
}								t_token;

/* ============================== REDIRS / AST ============================= */

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}								t_redir_type;

typedef struct s_redir
{
	t_redir_type				type;
	char						*filename;
	char						*delim;
	int							fd;
	bool						filename_can_expand;
	bool						delim_can_expand;
	struct s_redir				*next;
}								t_redir;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_AND,
	NODE_OR,
	NODE_SEQ,
	NODE_GROUP
}								t_node_type;

typedef struct s_ast
{
	t_node_type					type;
	char						**argv;
	bool						*argv_can_expand;
	t_redir						*redirs;
	struct s_ast				*left;
	struct s_ast				*right;
	struct s_ast				*child;
}								t_ast;

/* ================================ EXEC =================================== */

typedef struct s_exec
{
	int							prev_read;
	int							pipe_fd[2];
	int							fd_heredoc[2];
	char						*path;
	char						**argv;
	char						**envp;
	int							envp_built;
	t_redir						*redirs;
	struct s_data				*data;
}								t_exec;

/* ================================= ENV =================================== */

typedef struct s_env
{
	char						*key;
	char						*value;
	struct s_env				*next;
}								t_env;

/* ================================= APP =================================== */

typedef struct s_data
{
	char						*line;
	t_token						*tokens;
	t_ast						*ast;
	t_exec						*exec;
	t_env						*env;
	int							last_exit;
	int							input_status;
}								t_data;

/* ================================ LEXER ================================== */

t_token							*new_token_node(char *value);
bool							add_token_back(t_token **head, t_token *newtok);
t_token_type					get_token_type(char *str);
int								is_operator_start(char c);
int								get_operator_len(char *str);
char							*extract_word(t_data *data, int *i);
char							*extract_operator(t_data *data, int *i);
char							*extract_token(t_data *data, int *i);
int								tokenize_line(t_data *data);
void							skip_spaces(t_data *data, int *i);
char							*append_token_part(char *token, char *part);

/* ================================ UTILS ================================== */

void							print_tokens(t_token *tokens);
void							free_token_list(t_token **head);
void							print_syntax_error(char *msg);
void							free_data(t_data *data);
void							free_data_tmp(t_data *data);
int								read_input(t_data *data);

/* ================================= AST =================================== */

t_ast							*new_ast_node(t_node_type type);
bool							add_arg(t_ast *cmd, char *s, bool can_expand);
t_redir							*new_redir(t_redir_type type, char *text,
									bool can_exp);
void							add_redir(t_ast *cmd_node, t_redir *redir);
void							print_ast(t_ast *node, int depth);
void							free_ast(t_ast *node);
t_ast							*parse_command(t_token **tokp);
t_ast							*parse_group(t_token **tokens);
t_ast							*parse_pipeline(t_token **tokens);
t_ast							*parse_and_or(t_token **tokens);
t_ast							*parse_sequence(t_token **tokens);
int								is_redir_tok(t_token_type t);
int								is_binop_tok(t_token_type t);
int								can_start_cmd_tok(t_token_type t);
int								is_ctrl_token_tok(t_token_type t);
int								syntax_err(char *msg);
int								check_after_loop(int paren, t_token_type prev);
int								parser_syntax(t_token *t);
int								is_binop_tok(t_token_type t);
int								is_redir_tok(t_token_type t);
int								can_start_cmd_tok(t_token_type t);
int								ps_error(char *msg);
int								ps_eof_quote(char q);
int								ps_handle_paren_open(t_token **pt,
									t_token_type *prev, int *paren);
int								ps_handle_paren_close(t_token **pt,
									t_token_type *prev, int *paren);
int								ps_handle_separator(t_token **pt,
									t_token_type *prev);
int								ps_handle_binop(t_token **pt,
									t_token_type *prev);
int								ps_handle_redir(t_token **pt,
									t_token_type *prev);
int								ps_handle_word(t_token **pt,
									t_token_type *prev);

/* parse_line.c */
t_ast							*parse_entry(t_data *data);
/* ============================= EXEC / HEREDOC ============================ */

int								exec_ast(t_ast *node, t_data *data);
int								prepare_one_heredoc(t_redir *r, t_data *data);
int								prepare_all_heredocs(t_ast *node, t_data *data);
int								save_stdio(int *saved_in, int *saved_out);
void							restore_stdio(int saved_in, int saved_out);
int								dup2_and_close(int from, int to);

/* ============================ BUILTIN UTILS ============================== */

int								str_eq(char *a, char *b);
int								builtin_argc(char **argv);
void							builtin_err(char *cmd, char *msg);
void							builtin_err_arg(char *cmd, char *arg,
									char *msg);
int								builtin_is_numeric(char *s);
int								export_print_sorted_fd(t_env *lst, int fd);
void							export_print_sorted(t_env *lst);
int								parse_assignment(char *s, char **key,
									char **val, int *append);
/* ============================== BUILTINS ================================ */

int								is_builtin(char *name);
int								exec_builtin(t_data *data, t_ast *cmd);
int								builtin_echo(t_data *d, char **av);
int								builtin_cd(t_data *d, char **av);
int								builtin_pwd(t_data *d, char **av);
int								builtin_export(t_data *d, char **av);
int								builtin_unset(t_data *d, char **av);
int								builtin_env(t_data *d, char **av);
int								builtin_exit(t_data *d, char **av);

/* ============================== ENV (LIST) ============================== */

t_env							*env_new(char *key, char *value);
void							env_add_back(t_env **lst, t_env *node);
t_env							*env_find(t_env *lst, char *key);
void							free_env(t_env **lst);

/* ============================ ENV (ENV <-> LIST) ======================== */

t_env							*env_from_envp(char **envp);
char							**env_list_to_envp(t_env *lst);

/* ================================ ENV OPS =============================== */

int								id_is_valid(char *s);
int								parse_assignment(char *s, char **key,
									char **val, int *append);
char							*env_get(t_env *lst, char *key);
int								env_set(t_env **lst, char *key, char *value,
									int overwrite);
int								env_append(t_env **lst, char *key,
									char *suffix);
int								env_inc_shlvl(t_env **lst);
void							export_print_sorted(t_env *lst);

/* ================================= SIGNALS ============================== */

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

/* ================================= MISC ================================ */

t_data							data_init(int argc, char **argv, char **envp);
int								shell_process(t_data *data);
int								unset_is_valid_name(char *s);
void							print_env(t_env *lst);
int								unset_remove_key(t_env **env, char *name);
int								export_set_assignment(t_env **lst,
									char *assign);

/* ============================ EXPANSION (COMMON) ======================== */

void							quote_step(char c, int *in_s, int *in_d);
char							*str_append_free(char *dst, char *src);
char							*substr_dup(char *s, size_t pos, size_t len);

/* =========================== EXPANSION (VARS) =========================== */

int								var_name_start(char c);
int								var_name_char(char c);
size_t							var_name_len(char *s, size_t pos);
char							*var_value_dup(char *name, t_data *data);
char							*ms_expand_vars(char *s, t_data *data);
char							**expand_argv_vars(char **argv,
									bool *can_expand, t_data *data);
size_t							var_name_extract(char *s, size_t i,
									char **name_out);
char							*var_expand_value(char *name, t_data *data);

/* =========================== EXPANSION (GLOB) =========================== */

int								glob_match_name(char *name, char *pat);
void							str_array_insertion_sort(char **arr, size_t n);
char							**expand_argv_glob(char **argv);

/* ============================= REDIRS / PATH ============================ */

int								parse_redirection(t_ast *cmd, t_token **tokp);
char							*hd_strip_outer_quotes(char *s);
char							*heredoc_expand_line(char *line, t_data *data);
int								wait_and_get_status(pid_t pid);
int								apply_redirs(t_redir *rlist);
int								redirect_io(int in_fd, int out_fd);
int								wait_status_to_code(int s);
char							*find_cmd_path(char *name, t_env *env);
int								expand_redirs_inplace(t_redir *rlist,
									t_data *data);
char							**expand_argv_full(t_ast *cmd, t_data *data);
char							**expand_argv_dup(t_ast *cmd, t_data *data);

/* ============================= EXEC DETAILS ============================= */

// int								exec_group_node(t_ast *n, t_data *data);
// int								exec_pipeline_node(t_ast *n, t_data *data);
// int								exec_cmd_node(t_ast *n, t_data *data);
// int								exec_and_or_node(t_ast *n, t_data *d);
// int								exec_seq_node(t_ast *n, t_data *data);
void							child_exec_cmd(t_ast *cmd, t_exec *x);
int								run_subshell(t_ast *sub, t_data *data);
void							exec_ctx_init_cmd(t_exec *x, struct s_data *d,
									t_ast *cmd);
void							exec_ctx_close_pipes(t_exec *x);
void							exec_ctx_free_argv(t_exec *x);
void							exec_ctx_free_envp(t_exec *x);
void							exec_ctx_free_path(t_exec *x);
int								run_builtin_argv(t_data *data, char **av);
t_ast							*parse_entry(t_data *data);
void							set_exit_status(int code);
void							print_indent(int depth);
void							print_redirs(t_redir *r, int depth);
size_t							env_list_size(t_env *lst);

/* =========================== WILDCARD UTIL ARR ========================== */

int								match_star(char *name, char *pat);
int								pattern_has_star(char *s);
int								arr_grow(char ***arr, size_t *cap);
int								arr_push_dup(char ***arr, size_t *cap,
									size_t *n, char *s);
void							arr_free_all(char **arr, size_t n);
void							sort_strings(char **arr, size_t n);

#endif
