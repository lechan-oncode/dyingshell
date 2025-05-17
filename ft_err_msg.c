#include "minishell.h"

/*
err_msg.c
int err
0 = too many arguments
1 = not a valid identifier
*/

int ft_err_msg(char *builtin, char *msg, int err)
{
    write(2, "bash: ", 6);
    write(2, builtin, ft_strlen(builtin));
    write(2, ": ", 2);
    if (msg != NULL)
    {
        write(2,"'", 1);
        write(2, msg, ft_strlen(msg));
        write(2,"': ", 1);
    }
    if (err == 0)
    {
        write(2, "too many arguments\n", 19);
        return (1);
    }
    else if (err == 1)
    {
        write(2, "not a valid identifier\n", 25);
        return (1);
    }
    else if (err == 2)
    {
        write(2, "No such file or directory\n", 28);
        return (1);
    }
    return (0);
}