/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:07:56 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/28 15:33:41 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

/* --- !!! IMPORTANT: You MUST implement these BST utility functions !!! --- */

/**
 * @brief Prints the environment variables stored in the BST using
 * in-order traversal (usually alphabetical order by key).
 * Handles NULL values without using ternary operators.
 *
 * @param node The current node in the traversal (start by passing the root).
 */
void	print_env_bst(t_env_node *node)
{
	char	*value_to_print;

	if (node == NULL)
		return ;
	print_env_bst(node->left);
	if (node->key)
	{
		if (node->value != NULL)
			value_to_print = node->value;
		else
			value_to_print = "";
		printf("%s=%s\n", node->key, value_to_print);
	}
	print_env_bst(node->right);
}

/**
 * @brief Frees all memory associated with the environment BST.
 * Recommended implementation: Post-order traversal.
 */
void	free_env_bst(t_env_node *root)
{
	if (root == NULL)
		return ;
	free_env_bst(root->left);
	free_env_bst(root->right);
	free(root->key);
	free(root->value);
	free(root);
}

/**
 * @brief Initializes shell components: environment BST, signals, etc.
 * @param envp Environment variables from main.
 * @return Pointer to the root of the environment BST, or NULL on failure.
 * debug var: *m
 */
t_env_node	*init_shell(char **envp)
{
	t_env_node	*env_root;
	char		*m;

	env_root = init_env_bst(envp);
	if (!env_root)
		return (NULL);
	printf("--- Initial Environment Tree (BST In-Order) ---\n");
	print_env_bst(env_root);
	printf("--- End Environment Tree ---\n");
	m = get_env_value(env_root, "PATH");
	if (m)
		printf("DEBUG [init]: PATH found.\n");
	else
		printf("DEBUG [init]: PATH not found.\n");
	// TODO: Initialize signal handlers here
	return (env_root);
}

/**
 * @brief The main read-execute loop of the shell.
 * @param env_root Pointer to the environment BST root.
 * (Pass as t_env_node ** if execution needs to modify root ptr).
 */
void	shell_loop(t_env_node *env_root)
{
	char	*line;
	// t_cmd *cmd_head; // Declare here if used in loop
	(void)env_root;

	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (*line)
		{
			add_history(line);
			printf("DEBUG: cmd_checker result: %d\n", cmd_checker(line));
		}
		free(line);
	}
}

/**
 * @brief Cleans up shell resources before exiting.
 * @param env_root Pointer to the environment BST root.
 */
void	cleanup_shell(t_env_node *env_root)
{
	free_env_bst(env_root);
	rl_clear_history();
}
