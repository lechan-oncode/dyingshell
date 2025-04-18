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
    new_token->prev = NULL;
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

char    *get_spec_env_move(char *arg, int *i, int *j, t_vars *vars)
{
    (void)vars;

    if (arg[*i] == '?')
    {
        (*j)++;
        *i = *j;
        return ("error code");
    }
    return (NULL);
}
char *get_env_move(char *arg, int *i, int *j, t_vars *vars)
{
    char *key;
    t_env *current;

    if  (arg[*i] == '?')
        return (get_spec_env_move(arg, i, j, vars));
    while (arg[*j] == 95 || ft_isalnum(arg[*j])) 
        (*j)++;
    if (*j == 0)
        return (ft_calloc(1, sizeof(char)));
    key = ft_substr(arg, *i, *j - *i);
	*i = *j;
    current = vars->env_list;
    while (current->key)
    {
        if (ft_strncmp(key, current->key, ft_strlen(current->key)) == 0)
            return (free(key), ft_strdup(current->val));
        current = current->next;
    }
    return (free(key), ft_calloc(1, sizeof(char)));
}

char *ft_join_quote_expand_move(char *tmp, char *arg, int *i, int *j, t_vars *vars)
{
    char *result;
    char *env_val;
    char *substr;

    if (!tmp || !arg)
        return (NULL);
    result = ft_calloc(1, sizeof(char));
    if (*j > 0)
    {
        substr = ft_substr(arg, *i, *j - *i);
        result = ft_strjoin(tmp, substr);
        free(substr);
        free(tmp);
    }
    (*j)++;
    *i = *j;
    if (arg[*i - 1] == 36)
    {
        env_val = get_env_move(arg, i, j, vars);
        tmp = ft_strjoin(result, env_val);
        free(env_val);
        free(result);
        return (tmp);
    }
    return (result);
}

char *expand_quote(char *str, char *arg, int *i, int *j, t_vars *vars)
{
    char quote;

    quote = arg[*j];
    str = ft_join_quote_expand_move(str, arg, i, j, vars);
    while (arg[*j] && arg[*j] != quote)
    {
        if (arg[*j] == 36 && quote == 34)
        {
            str = ft_join_quote_expand_move(str, arg, i, j, vars);
            continue ;
        }
        (*j)++;
    }
    str = ft_join_quote_expand_move(str, arg, i, j, vars);
    return (str);
}

void expand_input(t_vars *vars)
{
    t_list *current;
    char *tmp;
    int start;
    int end;

    current = vars->tokens;
    while (current)
    {   
        start = 0;
        end = 0;
        tmp = ft_calloc(1, sizeof(char));
        while (current->str[end])
        {
            if (current->str[end] == 34 || current->str[end] == 39)
                tmp = expand_quote(tmp, current->str, &start, &end, vars);
            else if (current->str[end] == 36)
                tmp = ft_join_quote_expand_move(tmp, current->str, &start, &end, vars);
            else
                end++;
        }
        tmp = ft_join_quote_expand_move(tmp, current->str, &start, &end, vars);
        free(current->str);
        current->str = ft_strdup(tmp);
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
        printf("%d Token %d: %s\n", current->type, i++, current->str);
        current = current->next;
    }
}
void init_lst_type(t_list *list)
{
    t_list *current;

    current = list;
    while (current)
    {
        if (current->str[0] == 124)
            current->type = TYPE_PIPE;
        else if (current->str[0] == 60)
            if (current->str[1] == 60)
                current->type = TYPE_HEREDOC;
            else
                current->type = TYPE_REDIRECT_IN;
        else if (current->str[0] == 62)
            if (current->str[1] == 62)
                current->type = TYPE_APPEND;
            else
                current->type = TYPE_REDIRECT_OUT;
        else
            current->type = TYPE_CMD;
        current = current->next;
    }
}
char **join_cmd_token(t_list **current)
{
    int i;
    int j;
    t_list *tmp;
    char **cmd;

    if (!current || !*current)
        return (NULL);
    i = 0;
    j = 0;
    tmp = *current;        
    while (*current && (*current)->type == TYPE_CMD)
    {
        i++;
        *current = (*current)->next;
    }
    cmd = (char **)malloc(sizeof(char *) * (i + 1));
    if (!cmd)
        return (NULL);
    while (i > j)
    {
        cmd[j] = ft_strdup(tmp->str);
        tmp = tmp->next;
        j++;
    }
    cmd[j] = NULL;
    return (cmd);
}
void make_ast(t_vars *vars)
{
    t_list  *current;
    t_ast   *new_node;
    t_ast   *pipe_line;
    t_ast   *branch;

    init_lst_type(vars->tokens);
    current = vars->tokens;
    while (current)
    {
        new_node = (t_ast *)malloc(sizeof(t_ast));
        if (!new_node)
            return ;
        new_node->argv = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
        if (current->type == TYPE_CMD)
        {
            new_node->argv = join_cmd_token(&current);
            branch = new_node;
        }
        else if (current->type == TYPE_REDIRECT_IN || current->type == TYPE_REDIRECT_OUT || current->type == TYPE_APPEND || current->type == TYPE_HEREDOC)
        {
            new_node->left = branch;
            new_node->right = NULL;
            new_node->type = current->type;
        }
        else if (current->type == TYPE_PIPE)
        {
            new_node->type = TYPE_PIPE;
            if (pipe_line == NULL)
            {
                new_node->left = branch;
                new_node->right = NULL;
                pipe_line = new_node;
            }
            else
            {
                pipe_line->right = branch;
                new_node->left = pipe_line;
                new_node->right = NULL;
                pipe_line = new_node;
            }
            branch = NULL;
        }
        free(new_node);
        if (current != NULL)
            current = current->next;    
    }
    vars->ast = new_node;
}

int main(int ac, char **av, char **env)
{
    t_vars vars;

    (void)ac;
    (void)av;
    vars.args = NULL;
    vars.exit_status = 0;
    if (init_env(0, &vars, env))
        return (free_env(&vars), 0);
    while (1)
    {
        vars.tokens = NULL;
        parsing_input(&vars);
        expand_input(&vars);
        make_ast(&vars);
        read_tokens(&vars);
        free_list(vars.tokens);
    }
    
    free_env(&vars);
    free(vars.args);
    return (0);
}