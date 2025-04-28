/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 18:44:36 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/28 18:44:46 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

t_token	*check_operator(char c)
{

}

t_token	*tokenize(const char *input)
{
	t_token		*head;
	t_token		*tail;
	t_token		*new_token;
	int			len;
	const char	*start;

	head = NULL;
	tail = NULL;
	new_token = NULL;
	len = 0;
	if (!input)
		return (NULL);
	while (*input)
	{
		while (*input && ft_isspace(*input))
			input++;
		if (*input == '\0')
			break ;
		start = input;
		while (*input && !ft_isspace(*input))
		{
			if (ft_isoperator(*input))
			{
				input++;
				check_operator(*input);
			}
			input++;
		}
		len = input - start;
		if (len > 0)
		{
			new_token = creat_token(TOKEN_WORD, start, len);
			if (!new_token)
			{
				printf("Allocation for token is failes");
				free_token_list(head); //TODO
				return (NULL);
			}
			add_token_to_list(&head, &tail, new_token); //TODO
		}
	}
	return (head);
}

void	print_token_list(t_token *head)
{
	printf("Tokens:\n");
	while (head != NULL)
	{
		printf("  Type: %d, Value: [%s]\n", head->type, head->value ? head->value : "NULL");
		head = head->next;
	}
}
