/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_cheker.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 15:37:11 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/22 14:48:13 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//  ---get command --- check if builtin |--- yes
//                                      |--- ./path

#include "../../inc/minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)(*s1) - (unsigned char)(*s2));
}

int	cmd_checker(char *cmd)
{
	if (!cmd)
		return (0);
	char *argc1[] = {"cd","echo","pwd","exit"};    // check len after in while loop
	int i = 0;
	while (i < 4) // <----------here
	{
		if (ft_strcmp(cmd, argc1[i]) == 0)
			return (0);
		i++;
	}
	return (-1);
}
