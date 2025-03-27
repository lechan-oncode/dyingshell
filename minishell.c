# include "minishell.h"

void    parsing_input(t_vars *vars)
{
    char    *input;
    // char    **cmd;
    // int     i;

    vars->end = 1;
    while (vars->end)
    {
        input = readline("minishell$ ");
        printf("input : %s\n", input);
        add_history(input);
        ft_printf("input123: %s\n", input);
    }
}

int    init_env(t_vars *vars, char **env)
{
    if (!env || !*env || !vars)
        return (1);
    vars->env_list = (t_env *)malloc(sizeof(t_env));
    if (!vars->env_list)
       return (1) ;
    vars->env_list->prev = NULL;
    while (*env)
    {
        vars->env_list->val = *env;
        // vars->env_list->var = (split_var(*env), '=')[0];
        // vars->env_list->name = (split_var(*env), '=')[1];
        vars->env_list->next = (t_env *)malloc(sizeof(t_env));
        if (!vars->env_list->next)
            return (1);
        vars->env_list->next->prev = vars->env_list;
        vars->env_list = vars->env_list->next;
        env++;
    }
    vars->env_list->next = NULL;
    return (0);
}

int main (int ac, char **av, char **env)
{
    t_vars  vars;

    (void)ac;
    (void)av;
    init_env(&vars, env);
    // while (vars.env_list->prev)
    // {
    //     printf("vars.env_list : %s\n", vars.env_list->val);
    //     vars.env_list = vars.env_list->prev;
    // }>env_list->var = (split_var(*env), '=')[0];
    parsing_input(&vars);

    return (0);
}