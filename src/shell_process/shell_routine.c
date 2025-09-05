#include "minishell.h"

int	shell_process(t_data *data)
{
	if (tokenize_line(data) == 0)
	{
		print_tokens(data->tokens);
		data->ast = parse_sequence(&data->tokens);
		if (data->ast)
		{
			print_ast(data->ast, 0);
			// if (exec_ast(data->ast, data) == 0)
			// 	return (0);
			// print_syntax_error("exec");
			// return (1);
			return (0);
		}
		print_syntax_error("parcer");
		return (1);
	}
	print_syntax_error("tokenization");
	return (1);
}