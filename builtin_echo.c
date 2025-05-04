/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/05/04 17:05:05 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

// int builtin_unset(char **args, t_vars *vars)
// {

// 	return (cmdcode);
// }	

int ft_getenv_pos(char *key, t_vars *vars)
{
	int i;
	int len;

	i = 0;
	len = ft_strlen(key);
	while (vars->exp_arr[i])
	{
		if (ft_strncmp(vars->exp_arr[i], key, len) == 0)
			return (i);
		i++;
	}
	return (-1);
}
void modify_env_arr(char *arg, char *key, int flag, t_vars *vars)
{
    int count;
	int size;
	int pos;
    char **tmp;
    char **env_ptr;
	char **tmp_ptr;
	
	size = 0;
    pos = ft_getenv_pos(key, vars);
	while (vars->exp_arr[size])
		size++;
    tmp = malloc(sizeof(char *) * (size + flag + 1));
	env_ptr = vars->exp_arr;
	tmp_ptr = tmp;
    count = 0;
    while (pos > count++)
        *tmp_ptr++ = *env_ptr++;
    if (flag == 0)
	{
        *tmp_ptr++ = ft_strdup(arg);
		free(*env_ptr++);
    } 
	else if (flag == 1)
        *tmp_ptr++ = ft_strdup(arg);
    else if (flag == -1)
		free(*env_ptr++);
    while (*env_ptr)
        *tmp_ptr++ = *env_ptr++;
    *tmp_ptr = NULL;
    vars->exp_arr = tmp;
}


int	valid_key(char *key)
{
	int i;

	i = 0;
	if (!key || !*key)
		return (1);
	if (ft_isdigit(key[0]))
		return (1);
	while (key[i])
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (1);
		i++;
	}
	return (0);
}

char *ft_trim(char *str, char c, int key, int equal,int value)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c && key == 1)
			return (ft_substr(str, 0, i));
		else if (str[i] == c && equal == 1)
			return (ft_strdup(&c));
		else if (str[i] == c && value == 1)
			return (ft_substr(str, i + 1, ft_strlen(str) - i));
		i++;
	}
	if (key == 1)
		return (ft_strdup(str));
	return (NULL);
}

void sort_export(t_vars *vars)
{
	char **sort_arr;
	int i;
	int j;
	char *tmp;

	dup_arr(&sort_arr, vars->exp_arr, 0);
	i = 0;
	while (sort_arr[i])
	{
		j = i + 1;
		while (sort_arr[j])
		{
			if (ft_strncmp(sort_arr[i], sort_arr[j], ft_strlen(sort_arr[i])) > 0)
			{
				tmp = sort_arr[i];
				sort_arr[i] = sort_arr[j];
				sort_arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
	i = 0;
	while (sort_arr[i])
	{
		if (ft_strchr(sort_arr[i], '='))
			printf("declare -x %s\n", sort_arr[i]);
		i++;
	}
}

int builtin_export(char **args, t_vars *vars)
{
	char *key;
	char *equal;
	int i;

	i = 0;
	vars->exit_status = 0;
	if (!args[1])
		sort_export(vars);
	else
	{
		while (args[++i])
		{
			printf("args[%d]: %s\n", i, args[i]);
			key = ft_trim(args[i],  '=', 1, 0, 0);
			equal = ft_trim(args[i],'=', 0, 1, 0);
			if (valid_key(key) == 1)
			{
				printf("export: '%s': not a valid identifier\n", key);
				vars->exit_status = 1;
			}
			else if (ft_getenv_pos(key, vars) < 0)
            {
				printf("add key: %s\n", key);
				modify_env_arr(args[i], key, 1, vars);
			}
			else if (equal != NULL)
			{
				char *value = ft_trim(args[i], '=',0, 0, 1);
				printf("change value: %s\n", value);
				modify_env_arr(args[i], key, 0, vars);
			}
			free(key);
			free(equal);
		}
	}
	return (vars->exit_status);
}

int	builtin_echo(char **args, t_vars *vars)
{
	int i;
	int cmdcode;
	
	cmdcode = 0;
	i = 1;
	if (!args || !*args)
		return (1);
	if (args[i] && ft_strncmp(args[i], "-n", 2) == 0)
		i++;
	while (args[i])
	{
		printf("%s", args[i++]);
		if (args[i - 1][0] != '\0' && args[i])
			printf(" ");
	}
	if (args[1] && ft_strncmp(args[1], "-n", 2) != 0)
		printf("\n");
	vars->exit_status = cmdcode;
	return (cmdcode);
}

int	builtin_pwd(char **args, t_vars *vars)
{
	char	*cwd;

	cwd = ft_getenv(args[0], vars);
	printf("%s\n", cwd);
	if (cwd)
		vars->exit_status = 1;	
	free(cwd);
	vars->exit_status = 0;
	return (vars->exit_status);
}

int builtin_env(t_vars *vars)
{
	int	i;
	int	cmdcode;

	i = 0;
	cmdcode = 0;
	if (!vars || !vars->exp_arr)
	{
		cmdcode = 127;
		return (cmdcode);
	}
	while (vars->exp_arr[i] && ft_strchr(vars->exp_arr[i], '='))
	{
		printf("%s\n", vars->exp_arr[i]);
		i++;
	}
	vars->exit_status = cmdcode;
	return (cmdcode);
}