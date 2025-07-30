#include "minishell.h"

int	main(int argc, char **argv, char **env)
{
	char	*line;
	t_token	token;
	int		i;

	(void)argc;
	(void)argv;
	(void)env;
	i = 0;
    int j = 0;
    int y = 0;
	while (1)
	{
		memset(&token, 0, sizeof(t_token));
		line = readline("Minishell > ");
		while (line[i])
		{
            while (line[i] == ' ')
                i++;
            while (line[i])
            {
                int start = i;
                while (isalpha(line[i]))
                {
                    token.cmd[j][y] = line[i];
                    i++;
                }
                // This product is amazing, but the delivery was late.
                // Après la tokenisation :["This", "product", "is", "amazing", ",", "but", "the", "delivery", "was", "late", "."]

            }
            
		}
        for(int i = 0; token.cmd[i]; i++)
            printf("%s\n", token.cmd[i]);
		free(line);
	}
}
/*
Flag pour supprimer les leaks de readline
--suppressions=supp.supp
*/

/*
Tokenization
Parcer -> AST
*/