#include "minishell.h"

int join_list(t_list **head, t_list *new)
{
    t_list *current;

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
    t_list *new_token;
    int i;

    if (*len == 0 || vars == NULL || *start < 0 || vars->args == NULL)
        return (1);
    new_token = (t_list *)malloc(sizeof(t_list));
    if (!new_token)
        return (1);
    new_token->next = NULL;
    new_token->str = malloc(sizeof(char) * (*len + 1));
    if (!new_token->str)
        return (free(new_token), 1);
    i = 0;
    while (i < *len)
    {
        new_token->str[i] = vars->args[*start + i];
        i++;
    }
    new_token->str[i] = '\0';
    if (join_list(&vars->tokens, new_token))
        return (free(new_token), 1);
    *start += *len;
    *len = 0;
    return (0);
}

int clean_space(int i, int j, t_vars *vars)
{
    const char *space = " \t\n\r\v\f";
    char c;
    char *input;

    if (vars == NULL || vars->args == NULL)
        return (1);
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
        else if (input[i + j] == 60 || input[i + j] == 62 || input[i + j] == 124)
        {
            if (j > 0)
            {
                if (new_token(&i, &j, vars))
                    return (free(input), 1);
            }
            j++;
            if (input[i + j] == 60 || input[i + j] == 62)
                j++;
            new_token(&i, &j, vars);
            continue;
        }
        else if (ft_strchr(space, input[i + j]))
        {
            if (j > 0)
            {
                if (new_token(&i, &j, vars))
                    return (free(input), 1);
            }
            while (input[i + j] && ft_strchr(space, input[i + j]))
                i++;
            continue;
        }
        else
            j++;
        if (!input[i + j] && j > 0)
        {
            if (new_token(&i, &j, vars))
                return (free(input), 1);
            break;
        }
    }
    return (0);
}

int parsing_input(t_vars *vars)
{
    vars->args = readline("minishell$ ");
    add_history(vars->args);
    if (clean_space(0, 0, vars))
        return (1);
    return (0);
}

int init_env(int i, t_vars *vars, char **env)
{
    t_env *head;
    t_env *current;

    if (!env || !*env || !vars)
        return (1);
    head = (t_env *)malloc(sizeof(t_env));
    if (!head)
        return (1);
    current = head;
    current->prev = NULL;
    while (env[i])
    {
        current->vari = ft_strdup(env[i]);
        current->key = ft_split((env[i]), '=')[0];
        current->val = ft_split((env[i]), '=')[1];
        current->index = i++;
        current->next = (t_env *)malloc(sizeof(t_env));
        if (!current->next)
            return (1);
        current->next->prev = current;
        current = current->next;
    }
    current->next = NULL;
    vars->env_list = head;
    return (0);
}

void free_env(t_vars *vars)
{
    t_env *current;
    t_env *next;

    current = vars->env_list;
    while (current)
    {
        next = current->next;
        free(current->key);
        free(current->vari);
        free(current->val);
        free(current);
        current = next;
    }
}
void free_list(t_list *list)
{
    t_list *current;
    t_list *next;

    current = list;
    while (current)
    {
        next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
}

char *get_env_val(char *arg, int *i, int *j, t_vars *vars)
{
    char *env_val;
    char *key;
    t_env *current;

    env_val = ft_calloc(1, sizeof(char));
    while (arg[*j] == 95 || ft_isalnum(arg[*j])) 
        (*j)++;
    key = ft_substr(arg, *i, *j - *i);
    current = vars->env_list;
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            env_val = ft_strdup(current->val);
            free(key);
            return (env_val);
        }
        current = current->next;
    }
    free(key);
    return (env_val);
}
char *ft_join_quote_expand_move(char *tmp, char *arg, int *i, int *j, t_vars *vars)
{
    char *result;
    char *env_val;
    char *substr;
    int  move;

    if (!tmp || !arg)
        return (NULL);
    substr = ft_substr(arg, *i, *j - *i);
    result = ft_strjoin(tmp, substr);
    (*j)++;
    *i = *j;
    free(substr);
    free(tmp);
    if (arg[*i - 1] == '$')
    {
        env_val = get_env_val(arg, *i, *j, vars);
        return (free(env_val), ft_strjoin(result, env_val));
    }
    return (result);
}
char *expand_quote(char *str, char *arg, int *i, int *j, t_vars *vars)
{
    char quote;
    char *tmp;

    quote = arg[*j];
    str = ft_join_move(str, vars, i, j, 1);
    while (arg[*j] && arg[*j] != quote)
    {
        if (arg[*j] == 36) // 36 is $
            str = ft_join_move(str, vars, i, j, 1);
        (*j)++;
    }
    free(str);
    str = tmp;
    return (str);
}

void expand_input(int start, int end, t_vars *vars)
{
    t_list *current;
    char *tmp;

    tmp = ft_calloc(1, sizeof(char));
    current = vars->tokens;
    while (current)
    {
        while (1)
        {
            if (current->str[end] == 34 || current->str[end] == 39) // 34 is " 39 is '
                tmp = expand_quote(tmp, current->str, &start, &end, vars);
            // else if (current->str[i] == 36) //36 is $

            // else if (current->str[i])

            else
                end++;
        }
        current = current->next;
    }
}

void read_tokens(t_vars *vars)
{
    t_list *current;

    int i = 0;
    current = vars->tokens;
    while (current)
    {
        printf("Token %d: %s\n", i++, current->str);
        current = current->next;
    }
}

int main(int ac, char **av, char **env)
{
    t_vars vars;

    (void)ac;
    (void)av;
    vars.args = NULL;
    if (init_env(0, &vars, env))
        return (free_env(&vars), 0);
    while (1)
    {
        vars.tokens = NULL;
        parsing_input(&vars);
        read_tokens(&vars);
        expand_input(0, 0, &vars);
        // read_tokens(&vars);
        free_list(vars.tokens);
    }
    free_env(&vars);
    free(vars.args);
    return (0);
}