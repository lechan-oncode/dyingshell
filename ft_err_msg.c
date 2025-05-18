#include "minishell.h"

/*
err_msg.c
int err
0 = too many arguments
1 = not a valid identifier
2 = Home not set
3 = OLDPWD not set
4 = numeric argument required
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
        write(2,"': ", 2);
    }
    if (err == 0)
        write(2, "too many arguments\n", 19);
    else if (err == 1)
        write(2, "not a valid identifier\n", 25);
    else if (err == 2)
        write(2, "HOME not set\n", 13);
    else if (err == 3)
        write(2, "OLDPWD not set\n", 15);
    else if (err == 4)
        write(2, "numeric argument required\n", 26);
    return (1);
}