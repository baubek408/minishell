/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:37:16 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/28 18:44:25 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"


int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' \
	|| c == '\n' || c == '\v' \
	|| c == '\f' || c == '\r');
}

int	ft_isoperator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

t_token	*creat_token(t_token_type type, const char *start, \
	size_t len)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
	{
		perror("Failed to malloc a token node");
		return (NULL);
	}
	new_token->value = ft_strndup(start, len);
	if (!new_token->value)
	{
		perror("strndup failed for token value");
		free(new_token);
		return (NULL);
	}
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

void	add_token_to_list(t_token **head, t_token **tail, t_token *new_token)
{
	if (!new_token)
		return ;
	if (*head == NULL)
	{
		*head = new_token;
		*tail = new_token;
	}
	else
	{
		(*tail)->next = new_token;
		*tail = new_token;
	}

}

void	free_token_list(t_token *head)
{
	t_token	*tmp;

	while (head != NULL)
	{
		tmp = head->next;
		free(head->value);
		free(head);
		head = tmp;
	}
}

