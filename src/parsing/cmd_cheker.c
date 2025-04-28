/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_cheker.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 15:37:11 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/28 15:37:43 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//  ---get command --- check if builtin |--- yes
//                                      |--- ./path

#include "../../inc/minishell.h"

int	cmd_checker(char *cmd)
{
	int		i;

	i = 0;
	if (!cmd)
		return (0);
	while (1)
	{
		if (ft_strcmp(cmd, "echo") == 0)
			return (0);
		else if (ft_strcmp(cmd, "cd") == 0)
			return (0);
		else if (ft_strcmp(cmd, "pwd") == 0)
			return (0);
		else if (ft_strcmp(cmd, "export") == 0)
			return (0);
		else if (ft_strcmp(cmd, "unset") == 0)
			return (0);
		else if (ft_strcmp(cmd, "env") == 0)
			return (0);
		else if (ft_strcmp(cmd, "ls") == 0)
			return (0);
		else if (ft_strcmp(cmd, "exit") == 0)
			return (0);
		else
			return (printf("command is not exist"), -1);
		i++;
	}
	return (-1);
}
