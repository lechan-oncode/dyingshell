/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/05/17 05:34:39 by codespace        ###   ########.fr       */
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
	int exit_code;

	i = 0;
	exit_code = 0;
	if (!args[1])
		sort_export(vars);
	else
	{
		while (args[++i])
		{
			key = ft_trim(args[i],  '=', 1, 0, 0);
			equal = ft_trim(args[i],'=', 0, 1, 0);
			if (valid_key(key) == 1)
				exit_code = ft_err_msg(args[0], args[i], 1);
			else if (ft_getenv_pos(key, vars) < 0)
            	modify_env_arr(args[i], key, 1, vars);
			else if (equal != NULL)
				modify_env_arr(args[i], key, 0, vars);
			free(key);
			free(equal);
		}
	}
	return (exit_code);
}

int builtin_unset(char **args, t_vars *vars)
{
	int		i;
	int		pos;
	char	*key;
	int		exit_code;
	
	i = 0;
	exit_code = 0;
	while (args[++i])
	{
		pos = ft_getenv_pos(args[i], vars);
		if (pos >= 0)
		{
			key = ft_trim(args[i], '=', 1, 0, 0);
			modify_env_arr(args[i], args[i], -1, vars);
			free(key);
		}
		else
			exit_code = ft_err_msg(args[0], args[i], 1);
	}
	return (exit_code);
}

int builtin_env(char **args, t_vars *vars)
{
	int i;
	char *equal;
	int exit_code;
	
	i = 0;
	exit_code = 0;
	if (args[1] != NULL)
		return (ft_err_msg(args[0], args[1], 2));
	while (vars->exp_arr[i])
	{
		equal = ft_trim(vars->exp_arr[i], '=', 0, 1, 0);
		if (equal != NULL)
			printf("%s\n", vars->exp_arr[i]);		
		free(equal);
		i++;	
	}
	return (exit_code);
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

char *ft_join_arr_delimit(char **arr, char c)
{
	int		i;
	char	*str;
	char	*tmp;
	char	*tmp2;
	char	*delimit;

	i = 0;
	str = ft_calloc(1,1);
	delimit = ft_strdup(&c);
	while (arr[i])
	{
		tmp = ft_strjoin(str, delimit);
		tmp2 = ft_strjoin(tmp, arr[i]);
		free(tmp);
		free(str);
		str = tmp2;
		i++;
	}
	if (ft_strncmp(str, "", 2) == 0)
		str = ft_strdup("/");
	free(delimit);
	return (str);
}

void pwd_clean_dot(char **pwd, int rd, char **tmp, int wrt)
{
	if (pwd[rd] == NULL)
	{
		free(pwd[rd]);
		return ;
	}
	else if (ft_strncmp(pwd[rd], ".", 2) == 0)
		pwd_clean_dot(pwd, rd + 1, tmp, wrt);
	else if (pwd[rd][0] == '\0')
		pwd_clean_dot(pwd, rd + 1, tmp, wrt);
	else if (ft_strncmp(pwd[rd], "..", 3) == 0)
	{
		if (wrt > 0)
		{
			free(tmp[wrt - 1]);
			tmp[wrt - 1] = NULL;
		}
		pwd_clean_dot(pwd, rd + 1, tmp, wrt - 1);
	}
	else
	{
		tmp[wrt] = ft_strdup(pwd[rd]);
		pwd_clean_dot(pwd, rd + 1, tmp, wrt + 1);
	}
}

char *handle_path(char *path, char *last_pwd)
{
	char	*new_pwd;
	char	**pwd_arr;
	char	**tmp_arr;
	char	*tmp;
	char	*tmp2;
	
	tmp_arr = (char **)malloc(sizeof(char *) * 100);
	if (path[0] == '/')
		tmp2 = ft_strdup(path);
	else if (last_pwd[0] == '/' && last_pwd[1] == '\0')
		tmp2 = ft_strjoin(last_pwd, path);
	else
	{
		tmp = ft_strjoin(last_pwd, "/");
		tmp2 = ft_strjoin(tmp, path);
		free(tmp);
	}
	pwd_arr = ft_split(tmp2, '/');
	free(tmp2);
	pwd_clean_dot(pwd_arr, 0, tmp_arr, 0);
	new_pwd = ft_join_arr_delimit(tmp_arr, '/');
	free_arr(pwd_arr);
	free_arr(tmp_arr);
	return (new_pwd);
}

int run_update_oldpwd_pwd(char *last_pwd, char *path, t_vars *vars)
{
	int		cmdcode;
	char	*tmp;
	char	*new_pwd;
	char	*new_oldpwd;

	cmdcode = 0;
	if (chdir(path) == -1)
	{
		printf("bash: cd: %s: No such file or directory\n", path);
		cmdcode = 1;
	}
	else
	{
		new_oldpwd = ft_strjoin("OLDPWD=", last_pwd);
		tmp = handle_path(path, last_pwd);
		new_pwd = ft_strjoin("PWD=", tmp);
		free(tmp);
		modify_env_arr(new_oldpwd, "OLDPWD", 0, vars);
		free(new_oldpwd);
		modify_env_arr(new_pwd, "PWD", 0, vars);
		free(new_pwd);
	}
	return (cmdcode);
}

int cd_oldpwd(char *pwd, char *oldpwd, t_vars *vars)
{
	int cmdcode;

	cmdcode = 0;
	if (oldpwd == NULL || oldpwd[0] == '\0')
	{
		printf("bash: cd: OLDPWD not set\n");
		cmdcode = 1;
	}
	else
		cmdcode = run_update_oldpwd_pwd(pwd, oldpwd, vars);
	return (cmdcode);
}

int cd_home(char *pwd, char *home, t_vars *vars)
{
	int cmdcode;

	cmdcode = 0;
	if (home == NULL || home[0] == '\0')
	{
		printf("bash: cd: HOME not set\n");
		cmdcode = 1;
	}
	else
		cmdcode = run_update_oldpwd_pwd(pwd, home, vars);
	return (cmdcode);
}

int builtin_cd(char **args, t_vars *vars)
{
	char 	*oldpwd;
	char	*pwd;
	char	*home;
	int		cmdcode;

	cmdcode = 0;
	oldpwd = ft_getenv("OLDPWD", vars);
	pwd = ft_getenv("PWD", vars);
	home = ft_getenv("HOME", vars);
	if (args[0] && args[1] && args[2])
	{
    	printf("bash: cd: too many arguments\n");
    	cmdcode = 1;
	}
	else if (args[1] == NULL || ft_strncmp(args[1], "~", 2) == 0)
		cmdcode = cd_home(pwd, home, vars);
	else if (ft_strncmp(args[1], "-", 2) == 0)
		cmdcode = cd_oldpwd(pwd, oldpwd, vars);
	else
		cmdcode = run_update_oldpwd_pwd(pwd, args[1], vars);
	free(home);
	free(oldpwd);
	free(pwd);
	return (cmdcode);
}

void free_exit(t_vars *vars)
{
	free_list(vars);
	free_ast(vars->ast);
	free_arr(vars->exp_arr);
}

int builtin_exit(char **args, t_vars *vars)
{
	int i;

	i = 0;
	if (args[0] && args[1] && args[2])
	{
		printf("bash: exit: too many arguments\n");
		return (1);
	}
	if (args[1] == NULL)
	{
		free_exit(vars);
		exit(0);
	}
	while (args[1][i])
	{
		if (!ft_isdigit(args[1][i++]))
		{
			printf("bash: exit: %s: numeric argument required\n", args[1]);
			exit(255);
		}
	}
	free_exit(vars);
	exit(ft_atoi(args[1]));
}