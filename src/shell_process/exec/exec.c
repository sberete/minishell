#include "minishell.h"

/* 1/5 : exécute une commande simple (builtin parent si pas de pipe) */
static int is_parent_builtin(char *name)
{
	return name
		&& (str_eq(name, "cd")
		 || str_eq(name, "export")
		 || str_eq(name, "unset")
		 || str_eq(name, "exit"));
}

static int wait_status_to_code(int s)
{
	if (WIFEXITED(s))
		return WEXITSTATUS(s);
	if (WIFSIGNALED(s))
		return 128 + WTERMSIG(s);
	return 1;
}

static void exec_cmd_in_child(t_ast *cmd, t_data *data)
{
	char **argv = expand_argv_dup(cmd, data);
	if (!argv || !argv[0])
		_exit(0);

	/* redirections */
	if (expand_redirs_inplace(cmd->redirs, data) != 0)
		_exit(1);
	if (apply_redirs(cmd->redirs) != 0)
		_exit(1);

	/* builtin dans l’enfant */
	if (is_builtin(argv[0])) {
		int st = run_builtin_argv(data, argv);
		_exit(st);
	}

	/* externe */
	{
		char  *path = find_cmd_path(argv[0], data->env);
		char **envp = env_list_to_envp(data->env);
		if (!path)
			_exit(127);
		execve(path, argv, envp);
		_exit(errno == ENOENT ? 127 : 126);
	}
}


int exec_cmd_node(t_ast *n, t_data *data)
{
	if (!n || n->type != NODE_CMD)
		return 1;

	/* Builtins parent (sans redirections) */
	if (n->argv && n->argv[0] && is_parent_builtin(n->argv[0]) && n->redirs == NULL)
	{
		char **av = expand_argv_dup(n, data);
		int    rc = av ? run_builtin_argv(data, av) : 1;
		data->last_exit = rc;
		return rc;
	}

	/* Sinon: fork et exécuter dans l’enfant (pas de re-fork) */
	pid_t pid = fork();
	if (pid < 0)
		return 1;

	if (pid == 0) {
		signals_setup_child();
		exec_cmd_in_child(n, data); /* ne revient pas */
		_exit(127);
	}
	signals_setup_parent_execwait();

	int s, rc;
	if (waitpid(pid, &s, 0) < 0) rc = 1;
	else rc = wait_status_to_code(s);

	data->last_exit = rc;
	return rc;
}



/* Convertit un status wait(2) en code shell 0..255 */
int run_subshell(t_ast *sub, t_data *data)
{
	pid_t pid;
	int   st;

	pid = fork();
	if (pid < 0)
		return 1;
	if (pid == 0) {
		signals_setup_child();
		st = exec_ast(sub, data);
		_exit(st);
	}
	signals_setup_parent_execwait();
	return wait_and_get_status(pid);
}
/* Exécute une commande *dans l’enfant* (pipeline/subshell) sans refork. */


/* 2/5 : exécute un pipeline binaire (gauche | droite), itératif */
static int exec_pipeline_node(t_ast *n, t_data *data)
{
	t_ast *nodes[256];
	int    count = 0;

	while (n && n->type == NODE_PIPE && count < 255) {
		nodes[count++] = n->left;
		n = n->right;
	}
	nodes[count++] = n;

	int   prev_in = -1, pfd[2];
	pid_t last_pid = -1;

	for (int i = 0; i < count; ++i) {
		if (i < count - 1 && pipe(pfd) != 0) {
			if (prev_in != -1) close(prev_in);
			return 1;
		}
		pid_t pid = fork();
		if (pid < 0) {
			if (prev_in != -1) close(prev_in);
			if (i < count - 1) { close(pfd[0]); close(pfd[1]); }
			return 1;
		}
		if (pid == 0) {
			signals_setup_child();
			if (prev_in != -1) {
				if (dup2(prev_in, STDIN_FILENO) < 0) _exit(1);
				close(prev_in);
			}
			if (i < count - 1) {
				close(pfd[0]);
				if (dup2(pfd[1], STDOUT_FILENO) < 0) _exit(1);
				close(pfd[1]);
			}
			if (nodes[i]->type == NODE_CMD)
				exec_cmd_in_child(nodes[i], data);
			else if (nodes[i]->type == NODE_GROUP)
				_exit(run_subshell(nodes[i]->child, data));
			else
				_exit(exec_ast(nodes[i], data));
		}
		if (prev_in != -1) close(prev_in);
		if (i < count - 1) {
			close(pfd[1]);
			prev_in = pfd[0];
		}
		last_pid = pid;
	}

	int s, rc = 0; pid_t w;
	while ((w = waitpid(-1, &s, 0)) > 0) {
		if (w == last_pid)
			rc = wait_status_to_code(s);
	}
	data->last_exit = rc;
	return rc;
}



/* 3/5 : exécute AND/OR */
static int exec_and_or_node(t_ast *n, t_data *d)
{
	int st = exec_ast(n->left, d);
	if (n->type == NODE_AND) {
		if (st == 0) st = exec_ast(n->right, d);
	} else {
		if (st != 0) st = exec_ast(n->right, d);
	}
	d->last_exit = st;
	return st;
}

/* 4/5 : séquence ';' */
static int exec_seq_node(t_ast *n, t_data *data)
{
	if (!n) return 1;
	(void)exec_ast(n->left, data);
	return exec_ast(n->right, data);
}


/* 5/5 : groupe '( ... )' dans un sous-shell */
static int exec_group_node(t_ast *n, t_data *data)
{
	if (!n) return 1;
	int st = run_subshell(n->child, data);
	data->last_exit = st;
	return st;
}

/* PUBLIC: wrapper top-level (prépare heredocs une seule fois) */
int	exec_ast(t_ast *node, t_data *data)
{
	static int	depth = 0;
	int			st;

	if (!node)
		return (0);
	if (depth == 0)
	{
		if (prepare_all_heredocs(node, data) != 0)
			return (130);
	}
	depth++;
	if (node->type == NODE_CMD)
		st = exec_cmd_node(node, data);   /* <-- plus d’anciens 5 arguments */
	else if (node->type == NODE_PIPE)
		st = exec_pipeline_node(node, data);
	else if (node->type == NODE_AND || node->type == NODE_OR)
		st = exec_and_or_node(node, data);
	else if (node->type == NODE_SEQ)
		st = exec_seq_node(node, data);
	else if (node->type == NODE_GROUP)
		st = exec_group_node(node, data);
	else
		st = 1;
	depth--;
	data->last_exit = st;
	return (st);
}
