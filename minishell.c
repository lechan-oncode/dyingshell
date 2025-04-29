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
    vars->env_list = NULL;

    if (vars->env_arr)
    {
        while (vars->env_arr && *vars->env_arr)
        {
            free(*vars->env_arr);
            vars->env_arr++;
        }
        free(vars->env_arr);
        vars->env_arr = NULL;
    }
}

void free_ast(t_ast *node)
{
    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->argv)
    {
        char **tmp = node->argv;
        while (*tmp)
        {
            free(*tmp);
            tmp++;
        }
        free(node->argv);
    }
    free(node);
}

void free_list(t_vars *vars)
{
    t_list *current;
    t_list *next;

    free(vars->args);
    current = vars->tokens;
    while (current)
    {
        next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
    vars->tokens = NULL;
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
        free(tmp);
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
            current->type = TYPE_ARGV;
        current = current->next;
    }
}

void add_cmd_lst(t_list **tokens)
{
    t_list *current;
    t_list *add_exec;

    current = *tokens;
    while (current)
    {
        if (current->prev == NULL)
        {
            add_exec = (t_list *)malloc(sizeof(t_list));
            add_exec->str = NULL;
            add_exec->type = TYPE_CMD;
            add_exec->prev = NULL;
            add_exec->next = current;
            current->prev = add_exec;
            *tokens = add_exec;
        }
        if (current->type == TYPE_PIPE)
        {
            add_exec = (t_list *)malloc(sizeof(t_list));
            if (!add_exec)
                return ;
            add_exec->str = NULL;
            add_exec->type = TYPE_CMD;
            add_exec->prev = current;
            current->next = add_exec;
            add_exec->next = NULL;
            if (current->next == NULL)
            {
                add_exec->next = current->next;
                current->next->prev = add_exec;
            }
        }
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
    i = 1;
    j = 0;
    tmp = *current;        
    while ((*current)->next && (*current)->next->type == TYPE_CMD)
    {
        i++;
        *current = (*current)->next;
    }
    cmd = (char **)malloc(sizeof(char *) * (i + 1));
    if (!cmd)
        return (NULL);
    while (i > j)
    {
        cmd[j++] = ft_strdup(tmp->str);
        tmp = tmp->next;
    }
    cmd[j] = NULL;
    return (cmd);
}

void print_ast(t_ast *node, int depth)
{
    if (!node)
        return;

    // Print indentation based on depth
    for (int i = 0; i < depth; i++)
        printf("  ");

    // Print the node type
    switch (node->type)
    {
        case TYPE_CMD:
            printf("COMMAND Node\n");
            break;
        case TYPE_PIPE:
            printf("PIPE Node\n");
            break;
        case TYPE_REDIRECT_IN:
            printf("REDIRECT_IN (<) Node\n");
            break;
        case TYPE_REDIRECT_OUT:
            printf("REDIRECT_OUT (>) Node\n");
            break;
        case TYPE_APPEND:
            printf("APPEND (>>) Node\n");
            break;
        case TYPE_HEREDOC:
            printf("HEREDOC (<<) Node\n");
            break;
        case TYPE_FILE:
            printf("FILE Node\n");
            break;
        default:
            printf("UNKNOWN Node\n");
            break;
    }

    // Print the arguments if they exist
    if (node->argv)
    {
        for (int i = 0; node->argv[i]; i++)
        {
            for (int j = 0; j < depth + 1; j++)
                printf("  ");
            printf("Arg[%d]: %s\n", i, node->argv[i]);
        }
    }


    // Recursively print the left and right children
    if (node->left)
    {
        for (int i = 0; i < depth; i++)
            printf("  ");
        printf("Left:\n");
        print_ast(node->left, depth + 1);
    }

    if (node->right)
    {
        for (int i = 0; i < depth; i++)
            printf("  ");
        printf("Right:\n");
        print_ast(node->right, depth + 1);
    }
}

void ast_redirect(t_ast **branch, t_list *current)
{
    t_ast *new_node;
    t_ast *tmp;

    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    new_node->argv = NULL;
    new_node->argv = (char **)malloc(sizeof(char *) * 2);
    new_node->argv[0] = ft_strdup("REDIRECT");
    new_node->argv[1] = NULL;
    new_node->type = current->type;
    new_node->prev = NULL;
    if (*branch == NULL)
        *branch = new_node;
    else if ((*branch)->type == 5)
    {
        new_node->right = (*branch);
        (*branch) = new_node;
    }   
    else 
    {
        tmp = *branch;
        while (tmp->left != NULL)
            tmp = tmp->left;
        tmp = new_node->right;
        new_node = tmp->prev->left;
    }
}

void ast_pipe(t_ast **pipe_line, t_ast **branch)
{
    t_ast *new_node;
    t_ast *tmp;

    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    new_node->left = NULL;
    new_node->type = TYPE_PIPE;
    new_node->argv = (char **)malloc(sizeof(char *) * 2);
    new_node->argv[0] = ft_strdup("PIPE");
    new_node->argv[1] = NULL;
    new_node->right = *branch;
    if (*pipe_line == NULL)
        *pipe_line = new_node;
    else
    {
        tmp = *pipe_line;
        while (tmp->left != NULL)
            tmp = tmp->left;
        tmp->left = new_node;
    }
    *branch = NULL;
}

void ast_file(t_list **current, t_ast **branch)
{
    t_ast *new_node;
    t_ast *temp;

    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    temp = *branch;
    new_node->argv = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->type = TYPE_FILE;
    new_node->argv = (char **)malloc(sizeof(char *) * 2);
    new_node->argv[0] = ft_strdup((*current)->str);
    new_node->argv[1] = NULL;
    while (temp->left != NULL)
        temp = temp->left;
    temp->left = new_node;
    new_node->prev = temp;
}

void ast_cmd(t_list **current, t_ast **branch)
{
    t_ast *new_node;

    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    new_node->argv = NULL;
    new_node->prev = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->type = TYPE_CMD;
    new_node->argv = join_cmd_token(current);
    *branch = new_node;
}

void    make_ast(t_vars *vars)
{
    t_list  *current;

    init_lst_type(vars->tokens);
    add_cmd_lst(&vars->tokens);
    current = vars->tokens;
    while (current != NULL)
    {
        if (current->type == TYPE_CMD && vars->branch == NULL)
            ast_cmd(&current, &vars->branch);
        else if (current->type == TYPE_CMD && vars->branch != NULL)
            ast_file(&current, &vars->branch);
        else if (current->type == TYPE_REDIRECT_IN || current->type == TYPE_REDIRECT_OUT 
                || current->type == TYPE_APPEND || current->type == TYPE_HEREDOC)
            ast_redirect(&vars->branch, current);
        else if (current->type == TYPE_PIPE)
            ast_pipe(&vars->pipe_line, &vars->branch);
        current = current->next;  
    }
    if (vars->pipe_line != NULL)
    {
        t_ast *tmp;
        tmp = vars->pipe_line;
        while (tmp->left)
            tmp = tmp->left;
        tmp->left = vars->branch;
        vars->ast = vars->pipe_line;
    }
    else
        vars->ast = vars->branch;
}

void arr_env_list(t_vars *vars)
{
    t_env *current;
    t_env *temp;

    int i;
    if (!vars || !vars->env_list)
        return;

    current = vars->env_list;
    i = 0;
    // Count the number of environment variables
    while (current)
    {
        i++;
        current = current->next;
    }

    vars->env_arr = (char **)malloc(sizeof(char *) * (i + 1));
    if (!vars->env_arr)
        return;
    temp = vars->env_list;
    i = 0;

    // Populate the env_arr array
    while (temp)
    {
        if (temp->vari) // Ensure vari is valid
        {
            vars->env_arr[i] = ft_strdup(temp->vari);
            i++;
        }
        temp = temp->next;
    }
    vars->env_arr[i] = NULL;
}

char *ft_getenv(char *key, t_env *env_list)
{
    t_env *current;

    if (!key || !env_list)
        return (NULL);

    current = env_list;
    while (current)
    {
        if (ft_strncmp(key, current->key, ft_strlen(key) + 1) == 0)
            return (current->val);
        current = current->next;
    }
    return (NULL);
}
void execute_builtin(t_ast *node, t_vars *vars)
{
    if (ft_strncmp(node->argv[0], "echo", 5) == 0)
        builtin_echo(node->argv);
    // else if (ft_strncmp(node->argv[0], "cd", 3) == 0)
    //     ft_cd(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "pwd", 4) == 0)
        builtin_pwd(vars);
    // else if (ft_strncmp(node->argv[0], "export", 7) == 0)
    //     ft_export(node->argv, vars);
    // else if (ft_strncmp(node->argv[0], "unset", 6) == 0)
    //     ft_unset(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "env", 4) == 0)
        builtin_env(vars);
    // else if (ft_strncmp(node->argv[0], "exit", 5) == 0)
    //     ft_exit(vars);
}

int is_builtin(char *cmd)
{
    if (ft_strncmp(cmd, "echo", 5) == 0 || ft_strncmp(cmd, "cd", 3) == 0 ||
        ft_strncmp(cmd, "pwd", 4) == 0 || ft_strncmp(cmd, "export", 7) == 0 ||
        ft_strncmp(cmd, "unset", 6) == 0 || ft_strncmp(cmd, "env", 4) == 0 ||
        ft_strncmp(cmd, "exit", 5) == 0)
        return (1);
    return (0);
}

void run_exec(char *valid_path, t_ast *node, t_vars *vars)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execve(valid_path, node->argv, vars->env_arr) == -1)
        {
            perror("minishell");
            exit(1);
        }
    }
    else if (pid < 0)
        perror("minishell: fork failed");
    else
        waitpid(pid, &vars->exit_status, 0);
}

void execute_cmd(t_ast *node, t_vars *vars)
{
    char **path_list;
    char *path;
    char *valid_path;
    
    if (!node || !node->argv)
        return ;
    // printf("Executing command: %s\n", node->argv[0]);
    path_list = ft_split(ft_getenv("PATH", vars->env_list), ':');
    while  (*path_list)
    {
        path = ft_strjoin(*path_list, "/");
        valid_path = ft_strjoin(path, node->argv[0]);
        if (access(valid_path, X_OK) == 0)
        {
            // printf("Executable found: %s\n", ft_strjoin(path, node->argv[0]));
            run_exec(valid_path, node, vars);
            break;
        }
        free(valid_path);
        free(path);
        path_list++;
    }
}

void execute(t_ast *node, t_vars *vars)
{
    // printf("Executing command...\n");
    if (!node)
        return;
    if (node->type == TYPE_CMD)
    {
        if (is_builtin(node->argv[0]))
            execute_builtin(node, vars);
        else
            execute_cmd(node, vars);
    }
    else if (node->type == TYPE_PIPE)
    {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1)
        {
            perror("minishell: pipe failed");
            return;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // Left child process
            close(pipe_fd[0]); // Close read end
            dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(pipe_fd[1]);
            execute(node->left, vars);
            exit(0);
        }
        else if (pid < 0)
        {
            perror("minishell: fork failed");
        }

        pid = fork();
        if (pid == 0)
        {
            // Right child process
            close(pipe_fd[1]); // Close write end
            dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to pipe
            close(pipe_fd[0]);
            execute(node->right, vars);
            exit(0);
        }
        else if (pid < 0)
        {
            perror("minishell: fork failed");
        }

        // Parent process closes pipe and waits for children
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(-1, &vars->exit_status, 0);
        waitpid(-1, &vars->exit_status, 0);
    }

    else if (node->type == TYPE_REDIRECT_IN || node->type == TYPE_REDIRECT_OUT ||
             node->type == TYPE_APPEND || node->type == TYPE_HEREDOC)
    {
        int fd = -1;
        int stdin_cpy;
        int stdout_cpy;

        stdin_cpy = dup(STDIN_FILENO);
        stdout_cpy = dup(STDOUT_FILENO);
        if (node->type == TYPE_REDIRECT_IN)
            fd = open(node->right->argv[0], O_RDONLY);
        else if (node->type == TYPE_REDIRECT_OUT)
            fd = open(node->right->argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else if (node->type == TYPE_APPEND)
            fd = open(node->right->argv[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
        
        if (node->type == TYPE_REDIRECT_IN)
            dup2(fd, STDIN_FILENO);
        else
            dup2(fd, STDOUT_FILENO);
        execute(node->right, vars);  // Execute the command after redirection
        close(fd);
        dup2(STDIN_FILENO, stdin_cpy);
        dup2(STDOUT_FILENO, stdout_cpy);
    }
}
          

int main(int ac, char **av, char **env)
{
    t_vars vars;

    (void)ac;
    (void)av;
    if (init_env(0, &vars, env))
        return (free_env(&vars), 0);
    while (1)
    {
        vars.env_arr = NULL;
        vars.args = NULL;
        vars.pipe_line = NULL;
        vars.branch = NULL;
        vars.tokens = NULL;
        parsing_input(&vars);
        expand_input(&vars);
        init_lst_type(vars.tokens);
        printf("Tokens:\n");
        read_tokens(&vars);
        add_cmd_lst(&vars.tokens);
        printf("Input: %s\n", vars.args);
        read_tokens(&vars);
        // make_ast(&vars);
        // print_ast(vars.ast, 0);
        arr_env_list(&vars);
        // execute(vars.ast, &vars);
        // read_tokens(&vars);
        // free_list(&vars);
        // free_ast(vars.ast);
    }
    free_env(&vars);
    return (0);
}