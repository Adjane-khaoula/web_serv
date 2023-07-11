#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

typedef struct s_list {
	char **av;
	int fd_pipe[2];
	int prev_fd_pipe;
	int prev_type;
	int type;
} t_list;

int ft_strlen(char *s){
	int l = 0;
	while (s[l])
		l++;
	return l;
}

void error(char *func, char *msg, char *var) {
	write(2, "error: ", 7);
	if (func)
		write(2, func, ft_strlen(func));
	if (msg)
		write(2, msg, ft_strlen(msg));
	if (var)
		write(2, var, ft_strlen(var));
	write(2, "\n", 1);
}

void fatal(char *func, char *msg, char *var) {
	error(func, msg, var);
	exit(1);
}

int init_list(t_list *list, char **av, int i) {
	int len, end = i;
	while (av[end] && strcmp(av[end], "|") != 0 && strcmp(av[end], ";") != 0)
		end++;
	len = end - i;
	if (len > 0) {
		list->av = &av[i];
		list->prev_type = list->type;
		if (!av[end])
			list->type = 0;
		else if (strcmp(av[end], "|") == 0)
			list->type = 1;
		else
			list->type = 2;
		av[end]  = NULL;
		list->prev_fd_pipe = list->fd_pipe[0];
	}
	return end;
}

void execute(t_list *list, char **env) {
	pid_t pid;
	if ((list->type == 1 || list->prev_type == 1) && pipe((list->fd_pipe)))
		fatal(NULL, "fatal", NULL);
	if ((pid = fork()) < 0)
		fatal(NULL, "fatal", NULL);
	if (pid == 0) {
		if (list->type == 1 && (dup2(list->fd_pipe[1], 1) < 0))
			fatal(NULL, "fatal", NULL);
		if (list->prev_type == 1 && (dup2(list->prev_fd_pipe, 0) < 0))
			fatal(NULL, "fatal", NULL);
		if (execve(list->av[0], list->av, env) < 0)
			fatal(NULL, "cannot execute ", list->av[0]);
	} else {
		if (list->type == 1 || list->prev_type == 1) {
			close(list->fd_pipe[1]);
			if (list->type != 1)
				close(list->fd_pipe[0]);
		}
		if (list->prev_type == 1)
			close(list->prev_fd_pipe);
		waitpid(pid, NULL, 0);
	}
}

int main(int ac, char **av, char **env)
{
	t_list list;
	int start, i = 0;
	while (i < ac && av[++i]) {
		start = i;
		i = init_list(&list, av, i);
		if (strcmp("cd", av[start]) == 0) {
			if (i - start != 2)
				error("cd: ", "bad arguments ", NULL);
			else if (chdir(list.av[1]) < 0)
				error("cd: ", "cannot change directory to ", list.av[1]);
		}
		else if (i > start)
			execute(&list, env);
	}
	return 0;
}