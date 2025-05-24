#include "minishell.h"

int g_exit_status;

char *ft_getenv(char *key, t_vars *vars)
{
    int i;
    char *equal_sign;

    i = 0;
    if (!key || !vars || !vars->exp_arr)
        return (NULL);
    while (vars->exp_arr[i])
    {
        if (ft_strncmp(key, vars->exp_arr[i], ft_strlen(key)) == 0 &&
            vars->exp_arr[i][ft_strlen(key)] == '=')
        {
            equal_sign = ft_strchr(vars->exp_arr[i], '=');
            if (equal_sign)
                return (ft_substr(equal_sign + 1, 0,
                        ft_strlen(equal_sign + 1)));
        }
        i++;
    }
    return (ft_calloc(1, 1));
}

void dup_arr(char ***arr, char **src)
{
    int i;

    i = 0;
    while (src[i])
        i++;
    *arr = (char **)malloc(sizeof(char *) * (i + 1));
    if (!arr)
        return;
    i = 0;
    while (src[i])
    {
        (*arr)[i] = ft_strdup(src[i]);
        i++;
    }
    (*arr)[i] = NULL;
}

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

static void handle_quotes(int *i, int *j, char *input)
{
    char quote;
    
    quote = input[*i + *j];
    (*j)++;
    while (input[*i + *j] && input[*i + *j] != quote)
        (*j)++;
    if (input[*i + *j] == quote)
        (*j)++;
}

static void handle_special_chars(int *i, int *j, char *input, t_vars *vars)
{
    if (*j > 0)
    {
        if (new_token(i, j, vars)) 
            return ;  // Early exit if error
    }
    (*j)++;
    if (input[*i + *j] == input[*i + *j - 1] && input[*i + *j] != 124)
        (*j)++;
    new_token(i, j, vars);
}

static void handle_spaces(int *i, int *j, char *input, t_vars *vars)
{
    if (*j > 0)
    {
        if (new_token(i, j, vars)) 
            return ;
    }
    while (input[*i + *j] && ft_isspace(input[*i + *j]))
        (*i)++;
}

static void handle_end_of_token(int *i, int *j, char *input, t_vars *vars)
{
    if (!input[*i + *j] && *j > 0)
    {
        if (new_token(i, j, vars))
        {
            free(input);
            return ;
        }
        return ;
    }
}

int clean_space(int i, int j, t_vars *vars)
{
    char *input;

    if (vars == NULL || vars->args == NULL)
        return (1);
    input = ft_strdup(vars->args);
    while (input[i + j])
    {
        if ((input[i + j] == 34) || (input[i + j] == 39))
            handle_quotes(&i, &j, input);
        else if (input[i + j] == 60 || input[i + j] == 62 || input[i + j] == 124)
        {
            handle_special_chars(&i, &j, input, vars);
            continue;
        }
        else if (ft_isspace(input[i + j]))
        {
            handle_spaces(&i, &j, input, vars);
            continue;
        }
        else
            j++;
        handle_end_of_token(&i, &j, input, vars);
    }
    return (0);
}

int parsing_input(t_vars *vars)
{
    vars->args = readline("minishell$ ");
    if (vars->args == NULL)
        (printf("exit\n"), exit(0));
    add_history(vars->args);
    if (clean_space(0, 0, vars))
        return (1);
    return (0);
}

void free_arr(char **arr)
{
    int i;

    i = 0;
    if (arr[i])
        free(arr[i]);
    free(arr);
    arr = NULL;
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
        return (ft_itoa(g_exit_status));
    }
    return (NULL);
}
char *get_env_move(char *arg, int *i, int *j, t_vars *vars)
{
    char    *key;
    char    *value;
    
    if  (arg[*i] == '?')
        return (get_spec_env_move(arg, i, j, vars));
    while (arg[*j] == 95 || ft_isalnum(arg[*j])) 
        (*j)++;
    if (*j == 0)
        return (ft_calloc(1, sizeof(char)));
    key = ft_substr(arg, *i, *j - *i);
	*i = *j;
    value = ft_getenv(key, vars);
    if (value != NULL)
        return (value);
    return (ft_calloc(1, sizeof(char)));
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

void add_cmd_lst_head(t_list **current, t_list **token)
{
    t_list *add_exec;

    add_exec = malloc(sizeof(t_list));
    if (!add_exec)
        return;
    add_exec->str = NULL;
    add_exec->type = TYPE_CMD;
    add_exec->prev = NULL;
    add_exec->next = *current;
    if (*current)
        (*current)->prev = add_exec;
    *token = add_exec;
    *current = add_exec->next;
}

void add_cmd_lst_pipe(t_list **current)
{
    t_list *add_exec;

    add_exec = malloc(sizeof(t_list));
    if (!add_exec)
        return;
    add_exec->str = NULL;
    add_exec->type = TYPE_CMD;
    add_exec->prev = *current;
    add_exec->next = (*current)->next;
    if ((*current)->next)
        (*current)->next->prev = add_exec;
    (*current)->next = add_exec;
    *current = add_exec->next;
}

void add_cmd_fil_lst(t_list **tokens)
{
    t_list *current;

    current = *tokens;
    while (current)
    {
        if (current->prev == NULL)
            add_cmd_lst_head(&current, tokens);
        else if (current->type == TYPE_PIPE)
            add_cmd_lst_pipe(&current);
        else if (current->type == TYPE_REDIRECT_IN || current->type == TYPE_REDIRECT_OUT
                || current->type == TYPE_APPEND || current->type == TYPE_HEREDOC)
        {
            if (current->next->type == TYPE_ARGV)
                current->next->type = TYPE_FILE;
            current = current->next;
        }
        else
            current = current->next;
    }
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
    t_ast *tmp_branch;

    tmp_branch = *branch;
    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    new_node->argv = NULL;
    new_node->argv = (char **)malloc(sizeof(char *) * 2);
    new_node->argv[0] = ft_strdup("REDIRECT");
    new_node->argv[1] = NULL;
    new_node->type = current->type;
    new_node->left = NULL;
    new_node->right = NULL;
    while (tmp_branch->left != NULL)
        tmp_branch = tmp_branch->left;
    tmp_branch->left = new_node;
    new_node->prev = tmp_branch;
}

void ast_pipe(t_ast **pipe_line, t_ast **branch)
{
    t_ast *new_node;

    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    new_node->type = TYPE_PIPE;
    new_node->argv = (char **)malloc(sizeof(char *) * 2);
    new_node->argv[0] = ft_strdup("PIPE");
    new_node->argv[1] = NULL;
    new_node->left = NULL;
    new_node->right = *branch;
    new_node->prev = NULL;
    if (*pipe_line)
    {
        new_node->prev = *pipe_line;
        (*pipe_line)->left = new_node;
    }
    *pipe_line = new_node;
    *branch = NULL;
}

void ast_file(t_list *current, t_ast **branch)
{
    t_ast *new_node;
    t_ast *tmp_branch;

    tmp_branch = *branch;
    new_node = (t_ast *)malloc(sizeof(t_ast));
    if (!new_node)
        return ;
    new_node->argv = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->type = TYPE_FILE;
    new_node->argv = (char **)malloc(sizeof(char *) * 2);
    new_node->argv[0] = ft_strdup(current->str);
    new_node->argv[1] = NULL;
    while (tmp_branch->left != NULL)
        tmp_branch = tmp_branch->left;
    tmp_branch->right = new_node;
    new_node->prev = tmp_branch;
}

char **join_cmd_ast(t_list *current)
{
    int count;
    t_list *tmp;
    char **argv;

    count = 0;
    tmp = current;
    while (tmp && tmp->type != TYPE_PIPE)
    {
        if (tmp->type == TYPE_ARGV)
            count++;
        tmp = tmp->next;
    }
    argv = malloc(sizeof(char *) * (count + 1));
    if (!argv)
        return NULL;
    int i = 0;
    while (current && current->type != TYPE_PIPE)
    {
        if (current->type == TYPE_ARGV)
            argv[i++] = ft_strdup(current->str);
        current = current->next;
    }
    argv[i] = NULL;
    return (argv);
}


void ast_cmd(t_ast **branch, t_list *current)
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
    new_node->argv = join_cmd_ast(current);
    *branch = new_node;
}

void ast_return(t_ast **tmp_pipe, t_ast **branch, t_vars *vars)
{
    if (*tmp_pipe)
    {
        (*tmp_pipe)->left = *branch;
        (*branch)->prev = *tmp_pipe;
        while ((*tmp_pipe)->prev)
            *tmp_pipe = (*tmp_pipe)->prev;
        vars->ast = *tmp_pipe;
    }
    else
        vars->ast = *branch;
}

void    make_ast(t_vars *vars)
{
    t_list  *current;
    t_ast   *tmp_pipe;
    t_ast   *branch;

    tmp_pipe = NULL;
    branch = NULL;
    current = vars->tokens;
    while (current != NULL)
    {
        if (current->type == TYPE_CMD && branch == NULL)
            ast_cmd(&branch, current);
        else if (current->type == TYPE_FILE)
            ast_file(current, &branch);
        else if (current->type == TYPE_REDIRECT_IN || current->type == TYPE_REDIRECT_OUT 
                || current->type == TYPE_APPEND || current->type == TYPE_HEREDOC)
            ast_redirect(&branch, current);
        else if (current->type == TYPE_PIPE)
            ast_pipe(&tmp_pipe, &branch);
        current = current->next;
    }
    ast_return(&tmp_pipe, &branch, vars);
}

void sigint_child(int sig)
{
    (void)sig;
    write(2, "\n", 1);
    g_exit_status = 130;
}

void sigquit_child(int sig)
{
    (void)sig;
    write(2, "Quit: (core dumped)\n", 20);
    g_exit_status = 131;
}

void restore_signals(void)
{
    // signal(SIGINT, SIG_IGN);
    // signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, sigint_child);
    signal(SIGQUIT, sigquit_child);
}

int append(t_ast *node)
{
    int fd = -1;

    fd = open(node->right->argv[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("minishell: open failed");
        return (1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return (0);
}

int redirect_out(t_ast *node)
{
    int fd = -1;

    fd = open(node->right->argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("minishell: open failed");
        return (1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return (0);
}

int redirect_in(t_ast *node)
{
    int fd = -1;

    fd = open(node->right->argv[0], O_RDONLY);
    if (fd == -1)
    {
        perror("minishell: open failed");
        return (1);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
    return (0);
}
void heredoc(t_ast *node)
{
    int	pipe_ends[2];

    if (pipe(pipe_ends) == -1)
	    perror("minishell: heredoc failed");
    write(pipe_ends[1], node->argv[0], ft_strlen(node->argv[0]));
    close(pipe_ends[1]);
    dup2(pipe_ends[0], STDIN_FILENO);
    close(pipe_ends[0]);
}

int reccuring_redirection(t_ast *node)
{
    int cmdcode;

    cmdcode = 0;
    if (node->type == TYPE_REDIRECT_IN)
        cmdcode = redirect_in(node);
    else if (node->type == TYPE_REDIRECT_OUT)
        cmdcode = redirect_out(node);
    else if (node->type == TYPE_APPEND)
        cmdcode = append(node);
    else if (node->type == TYPE_HEREDOC)
       heredoc(node);
    if (node->left != NULL)
        reccuring_redirection(node->left);
    return (cmdcode);
}

void run_cmd(t_ast *node, t_vars *vars);

void execute_redirect(t_ast *node, t_vars *vars)
{   
    int stdin_cpy;
    int stdout_cpy;

    stdin_cpy = dup(STDIN_FILENO);
    stdout_cpy = dup(STDOUT_FILENO);
    reccuring_redirection(node);
    run_cmd(node->prev, vars);
    dup2(stdin_cpy, STDIN_FILENO);
    dup2(stdout_cpy, STDOUT_FILENO);
    close(stdin_cpy);
    close(stdout_cpy);
}

void run_exec(t_ast *node, t_vars *vars)
{
    char **path_list;
    char *path;
    char *valid_path;
    
    if (!node || !node->argv)
    return ;
    path_list = ft_split(ft_getenv("PATH", vars), ':');
    while  (*path_list)
    {
        path = ft_strjoin(*path_list, "/");
        valid_path = ft_strjoin(path, node->argv[0]);
        if (access(valid_path, X_OK) == 0)
        {
            execve(valid_path, node->argv, vars->exp_arr);
            // run_exec(valid_path, node, vars);
            break;
        }
        free(valid_path);
        free(path);
        path_list++;
    }
}

void run_cmd(t_ast *node, t_vars *vars)
{
    if (ft_strncmp(node->argv[0], "echo", 5) == 0)
        g_exit_status = builtin_echo(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "cd", 3) == 0)
        g_exit_status = builtin_cd(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "pwd", 4) == 0)
        g_exit_status = builtin_pwd(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "export", 7) == 0)
        g_exit_status = builtin_export(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "unset", 6) == 0)
        g_exit_status = builtin_unset(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "env", 4) == 0)
        g_exit_status = builtin_env(node->argv, vars);
    else if (ft_strncmp(node->argv[0], "exit", 5) == 0)
        g_exit_status = builtin_exit(node->argv, vars);
    else
        run_exec(node, vars);
}

void execute_cmd(t_ast *node, t_vars *vars)
{
    if (!node || !node->argv)
        return;
    // restore_signals();
    if (node->left != NULL)
        execute_redirect(node->left, vars);
    else
        run_cmd(node, vars);
}

void execute(t_ast *node, t_vars *vars)
{
    if (!node)
        return;

    int pipe_fd[2];
    int orig_stdout;
    int orig_stdin;
    int status;
        
    orig_stdout = dup(STDOUT_FILENO);
    orig_stdin = dup(STDIN_FILENO);
    if (pipe(pipe_fd) == -1)
    {
        perror("minishell: pipe failed");
        return;
    }
    restore_signals();

    // if (ft_strncmp(node->argv[0], "exit", 5) == 0)
    // {
    //     g_exit_status = builtin_exit(node->argv, vars);
    // }

    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipe_fd[0]); 
        if (node->type == TYPE_PIPE)
        {
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[1]);
            execute_cmd(node->right, vars);
        }
        else if (node->type == TYPE_CMD)
        {
            // close(pipe_fd[1]);
            execute_cmd(node, vars);
        }
        exit(0);
    }
    else if (pid < 0)
    {
        perror("minishell: fork failed");
    }
    if (pid > 0)
    {
        if (node->type == TYPE_PIPE && node->left != NULL)
		{
			close(pipe_fd[1]); // Close write end
			dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to pipe
			close(pipe_fd[0]);
            execute(node->left, vars);
		}

        
    }
    // Parent process closes pipe and waits for children
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    dup2(orig_stdout, STDOUT_FILENO); // Restore stdout
    dup2(orig_stdin, STDIN_FILENO); // Restore stdin
	close(orig_stdout);
	close(orig_stdin);
    waitpid(-1, &status, 0);
    
    if (WIFEXITED(status)) {
        g_exit_status = WEXITSTATUS(status);
    } else {
        // Handle abnormal termination
        g_exit_status = 1;
    }

    // if (ft_strncmp(node->argv[0], "exit", 5) == 0)
    // {
    //     free_list(vars);
    //     free_ast(vars->ast);
    //     free_arr(vars->exp_arr);
    //     exit(g_exit_status);
    // }
}

void signal_handler(int sigint)
{
    (void)sigint;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    g_exit_status = 130;
}

void init_signals(void)
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, SIG_IGN);
}

int main(int ac, char **av, char **envp)
{
    t_vars vars;

    (void)ac;
    (void)av;
    g_exit_status = 0;
    dup_arr(&vars.exp_arr, envp);
    while (1)
    {
        vars.args = NULL;
        vars.tokens = NULL;
        vars.ast = NULL;
        init_signals();
        parsing_input(&vars);
        expand_input(&vars);
        init_lst_type(vars.tokens);
        add_cmd_fil_lst(&vars.tokens);
        // read_tokens(&vars);
        make_ast(&vars);
        // print_ast(vars.ast, 0);


        if (ft_strncmp(vars.ast->argv[0], "exit", 5) == 0)
        {
            g_exit_status = builtin_exit(vars.ast->argv, &vars);
        }
        


        execute(vars.ast, &vars);
        free_list(&vars);
        free_ast(vars.ast);
    }
    free_arr(vars.exp_arr);
    return (0);
}