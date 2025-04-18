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
char	*ft_substr(char const *s, unsigned int start, size_t len);
void	*ft_calloc(size_t count, size_t size);
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlen(const char *s);
int     ft_isalnum(int i);
int     ft_atoi(const char *str);
int     ft_strncmp(const char *s1, const char *s2, size_t n);

typedef struct s_cmd
{
    char            *cmd;
    char            **args;
    int             fd_in;
    int             fd_out;
    int             pipefd[2];
    int             pid;
}   t_cmd;

typedef enum e_type
{
    TYPE_PIPE,
    TYPE_REDIRECT_IN,
    TYPE_REDIRECT_OUT,
    TYPE_APPEND,
    TYPE_HEREDOC,
    TYPE_CMD,
    TYPE_FILE
}   t_type;

typedef struct s_ast
{
    enum e_type     type;
    char            **argv;
    struct s_ast    *left;
    struct s_ast    *right;
}   t_ast;

typedef struct s_list
{
    char            *str;
    enum e_type     type;
    struct s_list   *prev;
    struct s_list   *next;
}   t_list;

typedef struct s_vars
{
    int             end;
    struct s_env    *env_list;
    struct s_list   *tokens;
    struct s_ast    *ast;
    char            *args;
    int             exit_status;
}   t_vars;

typedef struct  s_env
{
    int             index;
    char            *vari;
    char            *key;
    char            *val;
    struct s_env    *next;
    struct s_env    *prev;
}   t_env;

# endif