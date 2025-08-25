#include "libft.h"

char *ft_pathjoin(char const *s1, char const *s2)
{
    char *str;
    size_t i;

    if (!s1 || !s2)
        return NULL;

    str = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 2));
    if (!str)
        return NULL;

    i = 0;
    while (*s1)
        str[i++] = *s1++;
    if (str[i - 1] != '/')
        str[i++] = '/';
    while (*s2)
        str[i++] = *s2++;
    str[i] = '\0';
    return str;
}
