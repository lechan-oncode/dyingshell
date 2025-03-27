#ifndef MINISHELL_H
# define MINISHELL_H

//# include "libft.h"
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

typedef struct s_vars
{
    int             end;
    struct s_env    *env_list;
}   t_vars;

typedef struct  s_env
{
    char            *var;
    char            *name;
    char            *val;
    struct s_env    *next;
    struct s_env    *prev;
}   t_env;

# endif