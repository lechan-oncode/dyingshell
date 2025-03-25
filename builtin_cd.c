/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:26 by lechan            #+#    #+#             */
/*   Updated: 2025/03/23 04:15:19 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: cd. Changes the current working directory.
- Master control function for cd command.
- Handles special directories (home and previous).
- Changes to the directory specified in args[1].
- Updates PWD and OLDPWD environment variables.
- Frees memory allocated for oldpwd.
Returns 0 on success, 1 on failure.
*/
int	builtin_cd(char **args, t_vars *vars)
{
	char	*oldpwd;
	int		cmdcode;

	oldpwd = ft_strdup(get_env_val("OLDPWD", vars->env));
	if (!oldpwd)
		return (printf("cd: ft_strdup error\n"), 1);
	cmdcode = handle_cd_path(args, vars);
	if (cmdcode != 0)
	{
		ft_safefree((void **)&oldpwd);
		vars->error_code = cmdcode;
		return (cmdcode);
	}
	cmdcode = update_env_pwd(vars, oldpwd);
	if (cmdcode != 0)
	{
		ft_safefree((void **)&oldpwd);
		vars->error_code = cmdcode;
		return (1);
	}
	ft_safefree((void **)&oldpwd);
	vars->error_code = cmdcode;
	return (cmdcode);
}

/*
Handle changing to special directories (home or previous).
- Changes to HOME directory if no arguments or "~" is given.
- Changes to OLDPWD directory if "-" is given.
- Gets the path from the environment variables.
Returns 0 on success, 1 on failure.
*/
int	handle_cd_special(char **args, t_vars *vars)
{
	char	*path_value;
	int		cmdcode;

	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')))
	{
		path_value = get_env_val("HOME", vars->env);
		cmdcode = chdir(path_value);
		if (cmdcode != 0)
		{
			printf("cd: HOME not set or no access\n");
			return (1);
		}
		return (0);
	}
	path_value = get_env_val("OLDPWD", vars->env);
	cmdcode = chdir(path_value);
	if (cmdcode != 0)
	{
		printf("cd: OLDPWD not set or no access\n");
		return (1);
	}
	printf("%s\n", path_value);
	return (0);
}

/*
Handle directory change based on arguments.
- Changes to the directory specified in args[1].
- Prints error message if directory does not exist.
Returns 0 on success, 1 on failure.
*/
int	handle_cd_path(char **args, t_vars *vars)
{
	int	cmdcode;

	if (args[2])
	{
		ft_putendl_fd("cd: too many arguments\n", 2);
		return (1);
	}
	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')) ||
		(args[1][0] == '-' && args[1][1] == '\0'))
	{
		return (handle_cd_special(args, vars));
	}
	cmdcode = chdir(args[1]);
	if (cmdcode != 0)
	{
		ft_putendl_fd(" No such file or directory\n", 2);
		return (1);
	}
	return (0);
}

/*
Updates PWD and OLDPWD environment variables after directory change.
- Temporarily stores the old working directory in OLDPWD.
- Temporarily buffers the current working directory path in cwd[].
- Updates the PWD environment variable with the new path.
Returns 0 on success, 1 on failure.
*/
int	update_env_pwd(t_vars *vars, char *oldpwd)
{
	char	cwd[1024];
	char	*tmp;
	char	*result;

	tmp = ft_strjoin("OLDPWD=", oldpwd);
	if (!tmp)
		return (1);
	modify_env(&vars->env, 1, tmp);
	ft_safefree((void **)&tmp);
	result = getcwd(cwd, sizeof(cwd));
	if (result == NULL)
	{
		ft_putstr_fd("cd: error retrieving current directory\n", 2);
		return (1);
	}
	tmp = ft_strjoin("PWD=", cwd);
	if (!tmp)
		return (1);
	modify_env(&vars->env, 1, tmp);
	ft_safefree((void **)&tmp);
	return (0);
}
