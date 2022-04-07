#include <stdlib.h>
#include <unistd.h>
//#include <stdio.h>
//#include <string.h>
//#include <sys/types.h>

#define STDERR 2

typedef	struct	s_shame
{
	int		*pipe;
	int		*limit;
	int		n_pipes;
	int		fd[2][2];
	pid_t	*pid;
}		t_shame;

void	ft_exit_fatal(void)
{
	write(STDERR, "error: fatal\n", 13);
	exit (1);
}

int	ft_strlen(char *s)
{
	int	i = 0;

	while (s[i])
		i++;
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	if (!s1)
		return (0);
	while (*s1 == *s2++)
		if (*s1++ == 0)
			return (0);
	return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

char	*ft_strdup(char *s1)
{
	char	*ptr;
	int		i = -1;

	ptr = (char *)malloc(sizeof(char) * (ft_strlen(s1) + 1));
	if (!ptr)
		ft_exit_fatal();
	ptr[ft_strlen(s1)] = 0;
	while (s1[++i])
		ptr[i] = s1[i];
	return (ptr);
}

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*aux = (unsigned char *)b;
	size_t			i = 0;

	while (i < len)
		aux[i++] = c;
	return (aux);
}

void	ft_double_free(char **str)
{
	int	i = -1;
	
	while (str[++i])
		free(str[i]);
	free(str);
}

int	ft_cd(char **str)
{
	int	i = -1;

	while (str[++i])
	{
		if (!ft_strcmp(str[i], "cd"))
		{
			if (!str[i + 1] || str[i + 2])
			{
				write(STDERR, "error: cd: bad arguments\n", 25);
				return (1);
			}
			if (chdir(str[i + 1]) == -1)
			{
				write(STDERR, "error: cd: cannot change directory to ", 38);
				write(STDERR, str[i + 1], ft_strlen(str[i + 1]));
				write(STDERR, "\n", 1);
				return (1);
			}
		}
	}
	return (0);
}

void	ft_save_limit(char **str, t_shame *shame)
{
	int	i = 0;
	int	j = 0;

	while (str[++i])
		if (!ft_strcmp(str[i], ";"))
			j++;
	shame->limit = (int *)malloc((j + 1) * sizeof(int));
	shame->limit[j] = 0;
	i = 0;
	j = 0;
	while (str[++i])
		if (!ft_strcmp(str[i], ";"))
			shame->limit[j++] = i;
}

void	ft_save_pipe(char **str, t_shame *shame)
{
	int	i = -1;
	int	j = 0;

	while (str[++i])
		if (!ft_strcmp(str[i], "|"))
			j++;
	shame->pipe = (int *)malloc((j + 1) * sizeof(int));
	shame->pipe[j] = 0;
	shame->n_pipes = j + 1;
	i = 0;
	j = 0;
	while (str[++i])
		if (!ft_strcmp(str[i], "|"))
			shame->pipe[j++] = i;
	shame->pid = (pid_t *)malloc(sizeof(pid_t) * shame->n_pipes);
	if (pipe(shame->fd[0]))
		ft_exit_fatal();
	if (pipe(shame->fd[1]))
		ft_exit_fatal();
}

void	ft_pipes(char **str, t_shame *shame, char **env, int id)
{
	int i;
	
	ft_cd(str);
	if (id != 0)
		dup2(shame->fd[0][0], STDIN_FILENO);
	if (id != shame->n_pipes - 1)
		dup2(shame->fd[1][1], STDOUT_FILENO);
	close(shame->fd[0][0]);
	close(shame->fd[0][1]);
	close(shame->fd[1][0]);
	close(shame->fd[1][1]);
	i = execve(str[0], str, env);
	if (i == -1 && ft_strcmp(str[0], "cd"))
	{
		write(STDERR, "error: cannot execute ", 22);
		write(STDERR, str[0], ft_strlen(str[0]));
		write(STDERR, "\n", 1);
	}
	exit (1);
}

void ft_fork(t_shame *shame, char **str, char **env, int num)
{
	if (num < shame->n_pipes)
	{
		close(shame->fd[0][1]);
		shame->pid[num] = fork();
		if (shame->pid[num] == 0)
			ft_pipes(str, shame, env, num);
		else
		{
			waitpid(shame->pid[num], NULL, 0);
			close(shame->fd[0][1]);
			close(shame->fd[0][0]);
			shame->fd[0][0] = shame->fd[1][0];
			shame->fd[0][1] = shame->fd[1][1];
			if (pipe(shame->fd[1]))
				ft_exit_fatal();
		}
	}
}
int	main(int argc, char **argv, char **env)
{
	t_shame	shame;
	char	**temp;
	int		i = 1;
	int		w = 1;
	int		j = 0;
	int		k;
	int		x;

	char	**temp2;
	int 	num;
	int		i2;
	int		i3;
	int		j2;
	int		j3;
	int		k2;

	if (argc == 1)
		return (1);
	ft_memset(&shame, 0, sizeof(shame));
	ft_save_limit(argv, &shame);
	while (argv[i])
	{
		x = 0;
		k = 0;
		while (argv[i] && i != shame.limit[j])
		{
			k++;
			i++;
		}
		temp = (char **)malloc((k + 1) * sizeof(char *));
		if (!temp)
			ft_exit_fatal();
		temp[k] = 0;
		while (argv[w] && w != shame.limit[j])
			temp[x++] = ft_strdup(argv[w++]);
		ft_save_pipe(temp, &shame);
		i2 = 0;
		i3 = 0;
		j2 = 0;
		j3 = 0;
		num = 0;
		while (temp[i2])
		{
			k2 = 0;
			if (shame.pipe[0] != 0)
			{
				while (temp[i2])
				{
					if (i2 == shame.pipe[j2])
					{
						j2++;
						break ;
					}
					else
						k2++;
					i2++;
				}
				temp2 = (char **)malloc((k2 + 1) * sizeof(char *));
				if (!temp2)
					ft_exit_fatal();
				temp2[k2] = 0;
				k2 = 0;
				while (temp[i3])
				{
					if (i3 == shame.pipe[j3])
					{
						j3++;
						break ;
					}
					else
						temp2[k2++] = ft_strdup(temp[i3]);
					i3++;
				}
			}
			else
			{
				temp2 = temp;
				while (temp[i2])
				{
					i2++;
					i3++;
				}
			}
			ft_fork(&shame, temp2, env, num);
			num++;
			if (temp[i2])
			{
				i2++;
				i3++;
			}
			if (k2 !=  0)
				ft_double_free(temp2);
		}
		close(shame.fd[0][0]);
		close(shame.fd[0][1]);
		close(shame.fd[1][0]);
		close(shame.fd[1][1]);
		ft_double_free(temp);
		free(shame.pipe);
		free(shame.pid);
		if (argv[i])
		{
			i++;
			w++;
		}
		j++;
	}
	free(shame.limit);
	return (0);
}
