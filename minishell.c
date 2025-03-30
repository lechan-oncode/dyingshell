# include "minishell.h"

int join_list(t_list **head, t_list *new)
{
    t_list  *current;

    if (!head || !new)
        return (1);
    if (*head == NULL)
    {
        *head = new;
        return (0);
    }
    current = *head;
    while (current->next)
        current = current->next;
    current->next = new;
    new->prev = current;
    return (0);
}

int new_token(int *start, int *len, t_vars *vars)
{
    t_list  *new_token;
    int     i;

    if (*len == 0 || vars == NULL || *start < 0 || vars->args == NULL)
        return (1);
    new_token = (t_list *)malloc(sizeof(t_list));
    if (!new_token)
        return (1);
    new_token->next = NULL;
    new_token->str = malloc(sizeof(char) * (*len + 1));
    if (!new_token->str)
    {
        free(new_token);
        return (1);
    }
    i = 0;
    while(i < *len)
    {
        new_token->str[i] = vars->args[*start + i];
        i++;
    }
    new_token->str[i] = '\0';
    // printf("new_token : %s\n", new_token->str);
    if (join_list(&vars->tokens, new_token))
        return (1);
    *start += *len;
    *len = 0;
    return (0);
}

void clean_space(int i, int j, t_vars *vars)
{
    const char  *space = " \t\n\r\v\f";
    char        c;
    char        *input;

    input = ft_strdup(vars->args);
    while (input[i + j])
    {  
        if ((input[i + j] == 34) || (input[i + j] == 39))
        {
            c = input[i + j];
            j++;
            while (input[i + j] && input[i + j] != c)
                j++;
            if (input[i + j] == c)
                j++;
        }
        else if (ft_strchr(space, input[i + j]))
        {
            if (j > 0)
                new_token(&i, &j, vars);
            while (input[i + j] && ft_strchr(space, input[i + j]))
                i++;
            continue;
        }
        else
            j++;
        if (!input[i + j] && j > 0)
        {
            if (new_token(&i, &j, vars))
            {

                free(input);
                return ;
            }
            break;
        }
    }
}
void    parsing_input(t_vars *vars)
{
    vars->args = readline("minishell$ ");
    add_history(vars->args);
    clean_space(0, 0, vars);
}

int    init_env(t_vars *vars, char **env)
{
    t_env   *head;
    t_env   *current;
    int i;

    i = 0;
    if (!env || !*env || !vars)
        return (1);
    head = (t_env *)malloc(sizeof(t_env));
    if (!head)
       return (1) ;
    current = head;
    current->prev = NULL;
    while (env[i])
    {
        current->val = ft_strdup(env[i]);
        current->var = ft_split((env[i]), '=')[0];
        current->name = ft_split((env[i]), '=')[1];
        current->index = i;
        current->next = (t_env *)malloc(sizeof(t_env));
        if (!current->next)
            return (1);
        current->next->prev = current;
        current = current->next;
        i++;
    }
    current->next = NULL;
    vars->env_list = head;
    return (0);
}

int main (int ac, char **av, char **env)
{
    t_vars  vars;

    (void)ac;
    (void)av;
    vars.args = NULL;
    vars.tokens = NULL;
    init_env(&vars, env);
    while(vars.end)
    {
        parsing_input(&vars);
    }
    return (0);
}