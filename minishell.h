#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# include <limits.h>
# include <signal.h>
# include <string.h>
# include <termios.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h>
# include <sys/wait.h>

// Add libft function prototypes
char    *ft_strdup(const char *s);
char    **ft_split(char const *s, char c);
char	*ft_strchr(const char *s, int c);

typedef struct s_cmd
{
    char            *cmd;
    char            **args;
    int             fd_in;
    int             fd_out;
    int             pipefd[2];
    int             pid;
}   t_cmd;

typedef struct s_list
{
    char            *str;
    int             index;
    int             type;
    struct s_list   *prev;
    struct s_list   *next;
}   t_list;

typedef struct s_vars
{
    int             end;
    struct s_env    *env_list;
    struct s_list   *tokens;
    char            *args;
}   t_vars;

typedef struct  s_env
{
    int             index;
    char            *var;
    char            *name;
    char            *val;
    struct s_env    *next;
    struct s_env    *prev;
}   t_env;

# endif