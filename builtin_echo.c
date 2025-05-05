/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/05/05 18:07:48 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"



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
	int i;
	int j;
	char **tmp;
	
	i = 0;
	j = 0;
	count = 0;
	while (vars->exp_arr[count])
		count++;
    tmp = malloc(sizeof(char *) * (count + flag + 1));
	while (ft_getenv_pos(key, vars) > i)
		tmp[j++] = vars->exp_arr[i++];
	if (flag == -1 || flag == 0)
		i++;
	if (flag == 0)
        tmp[j++] = ft_strdup(arg);
	while (vars->exp_arr[i])
		tmp[j++] = vars->exp_arr[i++];
	if (flag == 1)
		tmp[j++] = ft_strdup(arg);
    tmp[j] = NULL;
	free_arr(vars->exp_arr);
	dup_arr(&vars->exp_arr, tmp);
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

void print_export(char **arr)
{
	int i;

	i = 0;
	while (arr[i])
	{
		printf("declare -x %s", ft_trim(arr[i], '=', 1, 0, 0));
		if (ft_strchr(arr[i], '='))
			printf("=\"%s\"", ft_trim(arr[i], '=', 0, 0, 1));	
		printf("\n");
		i++;
	}
}


int compare_strings(const char *s1, const char *s2)
{
    int i;
    
    i = 0;
	while (s1[i] != '\0' && s2[i] != '\0' && s1[i] != '=' && s2[i] != '=')
    {
        if (s1[i] > s2[i]) {
            return 1;
        } else if (s1[i] < s2[i]) {
            return -1;
        }
        i++;
    }
    if ((s1[i] == '=' && s2[i] != '=') || (s1[i] == '\0' && s2[i] != '=')) {
        return -1;
    } else if ((s2[i] == '=' && s1[i] != '=') || (s2[i] == '\0' && s1[i] != '=')) {
        return 1;
    }
    return 0;
}

void sort_export(t_vars *vars)
{
	char **sort_arr;
	int i;
	int j;
	char *tmp;

	dup_arr(&sort_arr, vars->exp_arr);
	i = 0;
	while (sort_arr[i])
	{
		j = i + 1;
		while (sort_arr[j])
		{
			if (compare_strings(sort_arr[i], sort_arr[j]) > 0)
			{
				tmp = sort_arr[i];
				sort_arr[i] = sort_arr[j];
				sort_arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
	print_export(sort_arr);
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
			key = ft_trim(args[i],  '=', 1, 0, 0);
			equal = ft_trim(args[i],'=', 0, 1, 0);
			if (valid_key(key) == 1)
				vars->exit_status = 1;
			else if (ft_getenv_pos(key, vars) < 0)
            	modify_env_arr(args[i], key, 1, vars);
			else if (equal != NULL)
				modify_env_arr(args[i], key, 0, vars);
			free(key);
			free(equal);
		}
	}
	return (vars->exit_status);
}

int builtin_unset(char **args, t_vars *vars)
{
	int i;
	int pos;
	char *key;
	
	i = 0;
	vars->exit_status = 0;
	while (args[++i])
	{
		pos = ft_getenv_pos(args[i], vars);
		if (pos >= 0)
		{
			key = ft_trim(args[i], '=', 1, 0, 0);
			modify_env_arr(args[i], args[i], -1, vars);
			free(key);
		}
	}
	return (vars->exit_status);
}

int builtin_env(t_vars *vars)
{
	int i;
	char *equal;
	
	i = 0;
	vars->exit_status = 0;
	while (vars->exp_arr[i])
	{
		equal = ft_trim(vars->exp_arr[i], '=', 0, 1, 0);
		if (equal != NULL)
			printf("%s\n", vars->exp_arr[i]);		
		free(equal);
		i++;	
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

	(void)args;
	cwd = ft_getenv("PWD", vars);
	printf("%s\n", cwd);
	if (cwd)
		vars->exit_status = 1;	
	free(cwd);
	vars->exit_status = 0;
	return (vars->exit_status);
}

int builtin_cd(char **args, t_vars *vars)
{
	char	*cwd;
	int		i;

	cwd = ft_getenv("HOME", vars);
	if (args[1] == NULL)
	{
		chdir(cwd);
		free(cwd);
		return (0);
	}
	else if (args[1][0] == '-' && args[1][1] == '\0')
	{
		i = ft_getenv_pos("OLDPWD", vars);
		if (i >= 0)
			chdir(vars->exp_arr[i]);
	}
	else
		chdir(args[1]);
	free(cwd);
	return (0);
}