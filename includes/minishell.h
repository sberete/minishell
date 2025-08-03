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
# include <unistd.h>

typedef enum e_token_type
{
	T_WORD, // ex: ls, -la, fichier.txt
	T_REDIR_IN,  // <
	T_REDIR_OUT, // >
	T_APPEND,    // >>
	T_HEREDOC,   // <<
	T_AND, // &&
	T_OR,  // ||
	T_PIPE, // |
	T_SEPARATOR, // ; (exécution séquentielle)
	T_PAREN_OPEN,  // (
	T_PAREN_CLOSE, // )
	T_WILDCARD, // *
	T_END, // Fin de commande, \0
	T_UNKNOWN // Tout ce qui ne devrait pas être là
}					t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}					t_token;

#endif