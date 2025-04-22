/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:43 by lechan            #+#    #+#             */
/*   Updated: 2025/04/22 11:46:24 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: env. Prints the environment variables.
- Prints all environment variables to STDOUT.
Returns 0 on success. Returns 1 on failure.
*/
int	builtin_env(t_vars *vars)
{
	int	i;
	int	cmdcode;

	i = 0;
	cmdcode = 0;
	if (!vars || !vars->env)
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
