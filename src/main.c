/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 15:35:09 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/25 17:07:45 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	main(int ac, char **av, char **envp)
{
	t_env_node	*env_root;

	(void)ac;
	(void)av;
	env_root = init_shell(envp);
	if (!env_root && *envp)
	{
		perror("Shell initialization failed");
		return (EXIT_FAILURE);
	}
	shell_loop(env_root);
	cleanup_shell(env_root);
	return (EXIT_SUCCESS);
}
