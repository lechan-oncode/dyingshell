/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/04/22 12:12:28 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

int	builtin_echo(char **args)
{
	int i;

	i = 1;
	if (!args || !*args)
		return (1);
	if (args[i] && ft_strncmp(args[i], "-n", 2) == 0)
		i++;
	while (args[i])
		printf("%s", args[i++]);
	if (args[1] && ft_strncmp(args[1], "-n", 2) == 0)
		return (0);
	else
		printf("\n");
	return (0);
}

int	builtin_pwd(t_vars *vars)
{
	char	*cwd;
	int		cmdcode;

	cmdcode = 0;
	if (!vars || !vars->env_arr)
	{
		cmdcode = 1;
		vars->exit_status = cmdcode;
		return (cmdcode);
	}
	// cwd = get_env_val("PWD", vars->env_arr);
	cwd = getcwd(NULL, 0);
	printf("%s\n", cwd);
	vars->exit_status = cmdcode;
	return (cmdcode);
}

int builtin_env(t_vars *vars)
{
	int	i;
	int	cmdcode;

	i = 0;
	cmdcode = 0;
	if (!vars || !vars->env_arr)
	{
		cmdcode = 1;
		return (cmdcode);
	}
	while (vars->env_arr[i])
	{
		printf("%s\n", vars->env_arr[i]);
		i++;
	}
	vars->exit_status = cmdcode;
	return (cmdcode);
}