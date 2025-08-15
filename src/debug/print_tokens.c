#include "minishell.h"

void	print_tokens(t_token *tokens)
{
	t_token	*tmp;

	tmp = tokens;
	printf("Tokens extraits :\n");
	while (tmp)
	{
		printf("Token: %-10s | Type: %d\n", tmp->value, tmp->type);
		tmp = tmp->next;
	}
}
