#include "minishell.h"
#include <string.h>
#include <unistd.h>

static int	is_executable(const char *p)
{
	return (access(p, X_OK) == 0);
}

char	*find_cmd_path(const char *name, t_env *env)
{
	char	*paths;
	char	**list;
	char	*ret;
	size_t	i;
	char	*tmp;
	char	*cand;

	ret = NULL;
	if (!name || !*name)
		return (NULL);
	/* chemin avec '/' -> tester directement */
	if (strchr(name, '/'))
		return (is_executable(name) ? ft_strdup(name) : NULL);
	paths = env_get(env, "PATH");
	if (!paths)
		return (NULL);
	list = ft_split(paths, ':');
	if (!list)
		return (NULL);
	for (i = 0; list[i]; ++i)
	{
		tmp = ft_strjoin(list[i], "/");
		cand = tmp ? ft_strjoin(tmp, name) : NULL;
		free(tmp);
		if (!cand)
			continue ;
		if (is_executable(cand))
		{
			ret = cand;
			break ;
		}
		free(cand);
	}
	free_tab(list);
	return (ret);
}
