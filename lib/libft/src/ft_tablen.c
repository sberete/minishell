#include "libft.h"

size_t	ft_tablen(char **t)
{
	size_t	n;

	if (!t)
		return (0);
	n = 0;
	while (t[n])
		n++;
	return (n);
}
