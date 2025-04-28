/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 16:38:53 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/25 16:41:57 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

/**
 * @brief Finds the value associated with a key in the environment BST.
 *
 * Performs a recursive search on the Binary Search Tree based on string
 * comparison of keys.
 *
 * @param root The root node of the BST to search within.
 * @param key The environment variable key (name) to search for.
 * @return char* A pointer to the value string if the key is found,
 * otherwise returns NULL. Returns NULL if root or key is NULL.
 * IMPORTANT: This function returns a pointer to the existing string
 * in the tree, NOT a new copy. Do not free the result
 * unless you intend to remove the node from the tree.
 * If you need a modifiable copy, use strdup/ft_strdup
 * on the returned pointer.
 */
char	*get_env_value(t_env_node *root, const char *key)
{
	int	cmp_result;

	if (root == NULL)
		return (NULL);
	if (key == NULL)
		return (NULL);
	cmp_result = ft_strcmp(key, root->key);
	if (cmp_result == 0)
		return (root->value);
	else if (cmp_result < 0)
		return (get_env_value(root->left, key));
	else
		return (get_env_value(root->right, key));
}
