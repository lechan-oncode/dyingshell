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

typedef enum e_type
{
    TYPE_PIPE,
    TYPE_REDIRECT_IN,
    TYPE_REDIRECT_OUT,
    TYPE_APPEND,
    TYPE_HEREDOC,
    TYPE_CMD,
    TYPE_ARGV,
    TYPE_FILE
}   t_type;

typedef struct s_ast
{
    enum e_type     type;
    char            **argv;
    struct s_ast    *prev;
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
    char            **exp_arr;
    struct s_list   *tokens;
    struct s_ast    *ast;
    char            *args;
}   t_vars;

// Add libft function prototypes
char    *ft_strdup(const char *s);
char    **ft_split(char const *s, char c);
char	*ft_strchr(const char *s, int c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
void	*ft_calloc(size_t count, size_t size);
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlen(const char *s);
int     ft_isalnum(int i);
int     ft_isdigit(int i);
int     ft_isspace(int i);
int     ft_atoi(const char *str);
int     ft_strncmp(const char *s1, const char *s2, size_t n);

int     builtin_echo(char **args, t_vars *vars);
int     builtin_env(char **args, t_vars *vars);
int     builtin_pwd(char **args, t_vars *vars);
int     builtin_export(char **args, t_vars *vars);
int     builtin_unset(char **args, t_vars *vars);
int     builtin_cd(char **args, t_vars *vars);
int     builtin_exit(char **args, t_vars *vars);
int     ft_getenv_pos(char *key, t_vars *vars);
void    modify_env_arr(char *arg, char *key, int flag, t_vars *vars);
int     valid_key(char *key);
void    free_exit(t_vars *vars);
void    free_ast(t_ast *node);
void    free_list(t_vars *vars);
void    print_export(char **arr);
void    sort_export(t_vars *vars);
void    free_arr(char **arr);
void    dup_arr(char ***arr, char **src);
char    *ft_trim(char *str, char c, int key, int equal, int value);
char    *ft_getenv(char *key, t_vars *vars);

int ft_err_msg(char *builtin, char *msg, int err);


# endif