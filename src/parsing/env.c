/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 15:38:38 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/25 16:38:39 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Helper function to create a new BST node
// Makes copies of key and value
t_env_node	*new_bst_node(const char *key, const char *value)
{
	t_env_node	*node;

	node = (t_env_node *)malloc(sizeof(t_env_node));
	if (!node)
	{
		perror("malloc failed for new env node");
		return (NULL);
	}
	node->key = strdup(key);
	node->value = strdup(value);
	if (!node->key || !node->value)
	{
		perror("strdup failed for key/value");
		free(node->key);
		free(node->value);
		free(node);
		return (NULL);
	}
	node->left = (NULL);
	node->right = (NULL);
	return (node);
}

/**
 * @brief Recursively inserts or updates a node in the BST.
 *
 * If key exists, updates value. Otherwise, inserts a new node.
 * Handles memory allocation for the new node and its contents.
 * Updates value in place if key exists.
 *
 * @param node Current node in the recursion.
 * @param key The environment variable key.
 * @param value The environment variable value.
 * @return The (potentially new) root of the subtree rooted at 'node'.
 */
t_env_node	*insert_or_update_bst_node(t_env_node *node, const char *key,
	const char *value)
{
	int		cmp;
	char	*new_value;

	if (node == NULL)
		return (new_bst_node(key, value));
	cmp = ft_strcmp(key, node->key);
	if (cmp < 0)
		node->left = insert_or_update_bst_node(node->left, key, value);
	else if (cmp > 0)
		node->right = insert_or_update_bst_node(node->right, key, value);
	else
	{
		new_value = ft_strdup(value);
		if (!new_value)
			perror("strdup failed for value update");
		else
		{
			free(node->value);
			node->value = new_value;
		}
	}
	return (node);
}

/**
 * @brief Public function to add/update an environment variable in the BST.
 *
 * Takes a pointer to the root pointer of the BST.
 *
 * @param root_ptr Pointer to the variable holding the root of the BST.
 * @param key The environment variable key.
 * @param value The environment variable value.
 */
void	add_env_bst(t_env_node **root_ptr, const char *key,
	const char *value)
{
	if (!root_ptr || !key || !value)
		return ;
	*root_ptr = insert_or_update_bst_node(*root_ptr, key, value);
}

// #include "libft.h" // Include your libft if using ft_ functions
// Include your t_env_node definition and add_env_bst function prototype here

/**
 * @brief Parses a single environment string and adds it to the BST.
 * Handles temporary memory allocation and freeing for key/value.
 * Assumes add_env_bst handles its own persistent allocation.
 *
 * @param root_ptr Pointer to the BST root pointer.
 * @param env_str The environment string (e.g., "KEY=VALUE").
 */
static void	process_and_add_env_str(t_env_node **root_ptr, const char *env_str)
{
	char	*equal_sign;
	char	*key;
	char	*val;
	int		key_len;

	equal_sign = ft_strchr(env_str, '=');
	if (equal_sign != NULL && equal_sign != env_str)
	{
		key_len = equal_sign - env_str;
		key = ft_strndup(env_str, key_len);
		val = ft_strdup(equal_sign + 1);
		if (key && val)
			add_env_bst(root_ptr, key, val);
		else
			perror("strdup failed during env init");
		free(key);
		free(val);
	}
}

/**
 * @brief Initializes the environment Binary Search Tree from the environ array.
 * Uses a helper function to process each string to stay within line limits.
 *
 * @param environ The environment array.
 * @return The root node of the newly created BST, or NULL.
 */
t_env_node	*init_env_bst(char **environ)
{
	t_env_node	*root;
	int			i;

	i = 0;
	root = NULL;
	if (!environ)
		return (NULL);
	while (environ[i] != NULL)
	{
		process_and_add_env_str(&root, environ[i]);
		i++;
	}
	return (root);
}
