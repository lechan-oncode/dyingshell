/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/05/04 13:08:23 by codespace        ###   ########.fr       */
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
	while (vars->env_arr[i])
	{
		if (ft_strncmp(vars->env_arr[i], key, len) == 0)
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
	while (vars->env_arr[size])
		size++;
    tmp = malloc(sizeof(char *) * (size + flag + 1));
	env_ptr = vars->env_arr;
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
    vars->env_arr = tmp;
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

int builtin_export(char **args, t_vars *vars)
{
	char *key;
	int i;

	i = 1;
	vars->exit_status = 0;
	if (args[1] == NULL)
		printf("sort export: \n");
	else
	{
		while (args[i])
		{
			key = ft_substr(args[i], 0, ft_strchr(args[i], '=') - args[i]);
			if (valid_key(key) == 1)
				vars->exit_status = 1;
			else if (ft_strncmp(ft_getenv(key, vars), "", 1) != 0)
				modify_env_arr(args[i], key, 0, vars);
			else if (valid_key(key) == 0)
				modify_env_arr(args[i], key, 1, vars);
			free(key);
			i++;
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
		if (args[i - 1][0] != '\0')
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
	if (!vars || !vars->env_arr)
	{
		cmdcode = 127;
		return (cmdcode);
	}
	while (vars->env_arr[i] && ft_strchr(vars->env_arr[i], '='))
	{
		printf("%s\n", vars->env_arr[i]);
		i++;
	}
	vars->exit_status = cmdcode;
	return (cmdcode);
}