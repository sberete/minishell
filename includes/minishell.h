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
	// Mots : commandes ou arguments
	T_WORD,            // ex: ls, -la, fichier.txt

	// Redirections
	T_REDIR_IN,        // <
	T_REDIR_OUT,       // >
	T_APPEND,          // >>
	T_HEREDOC,         // <<

	// Logique
	T_AND,             // &&
	T_OR,              // ||

	// Pipe
	T_PIPE,            // |

	// Séparateurs
	T_SEPARATOR,       // ; (exécution séquentielle)

	// Parenthèses pour sous-shells
	T_PAREN_OPEN,      // (
	T_PAREN_CLOSE,     // )

	// Wildcard
	T_WILDCARD,        // *

	// Fin de ligne / fin d'entrée
	T_END,             // Fin de commande, \0

	// Inconnu / non géré (pour debug ou erreurs)
	T_UNKNOWN          // Tout ce qui ne devrait pas être là
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

#endif