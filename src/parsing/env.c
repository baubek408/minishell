/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 15:38:38 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/22 14:33:10 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

char	*ft_strndup(const char *s, size_t n)
{
	size_t	len;
	char	*dup;
	size_t	i;

	len = 0;
	while (s[len] && len < n)
		len++;
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = s[i];
		i++;
	}
	dup[len] = '\0';
	return (dup);
}

t_env	*new_env_node(const char *key, const char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	// node->val = value ? ft_strdup(value) : NULL;
	if (value != NULL)
		node->val = ft_strdup(value);
	else
		node->val = NULL;
	node->next = NULL;
	return (node);
}

void	add_env_node(t_env **env_list, const char *key, const char *value)
{
	t_env	*new_node;
	t_env	*current;

	new_node = new_env_node(key, value);
	if (!new_node)
		return ;
	if (!*env_list)
	{
		*env_list = new_node;
		return ;
	}
	current = *env_list;
	while (current->next)
		current = current->next;
	current->next = new_node;
}


t_env	*init_env(char **line)
{
	t_env	*head;
	int		i;
	char	*c;
	int		key_len;
	char	*key;
	char	*val;

	head = NULL;
	i = 0;
	while (line[i])
	{
		c = ft_strchr(line[i], '=');
		if (c)
		{
			key_len = c - line[i];
			key = ft_strndup(line[i], key_len);
			val = ft_strdup(c + 1);
			add_env_node(&head, key, val);
			free(key);
			free(val);
		}
		i++;
	}
	return (head);
}

char	*get_env_value(t_env *env, const char *key)
{
	while (env)
	{
		if (strcmp(env->key, key) == 0)
			return (env->val);
		env = env->next;
	}
	return (NULL);
}
