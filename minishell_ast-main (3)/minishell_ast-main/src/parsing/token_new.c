/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_new.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:12:04 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 20:59:30 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

t_token	*init_token(const char *value)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->value = ft_strdup(value);
	new->type = -1;
	if (!new->value)
	{
		free(new);
		return (NULL);
	}
	new->next = (NULL);
	return (new);
}

void	add_token_back(t_token **head, t_token *new)
{
	t_token	*current;

	if (!*head)
	{
		*head = new;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new;
}


int	check_and_add_token(t_token **token_list, char *line, int start, int i)
{
	char	*clean_token;
	char	*raw_token;
	char	*expanded_token;

	raw_token = ft_strndup(line + start, i - start);
	expanded_token = expand_vars(raw_token);
	if (expanded_token)
		clean_token = strip_quotes(expanded_token);
	if (clean_token)
		add_token_back(token_list, init_token(clean_token));
	if (expanded_token)
		free(expanded_token);
	free(raw_token);
	return (0);
}

/**
 * @brief Finalizes the current token, processes it (expands, strips quotes),
 * and adds it to the token list.
 * Assumes expand_vars and strip_quotes return new strings or NULL.
 * Assumes init_token duplicates the string it receives.
 * @param list Pointer to the head of the token list.
 * @param s Pointer to the current parsing state.
 */
static void	finalize_and_add_token(t_token **list, t_parse_state *s)
{
	char	*raw_val;
	char	*expanded_val;
	char	*final_val;

	if (!s->token_in_progress || s->start_of_token >= s->i)
		return ;
	raw_val = ft_strndup(s->line + s->start_of_token, \
							s->i - s->start_of_token);
	if (!raw_val)
		return ;
	expanded_val = expand_vars(raw_val);
	free(raw_val);
	if (!expanded_val)
		return ;
	final_val = strip_quotes(expanded_val);
	free(expanded_val);
	if (!final_val)
		return ;
	add_token_back(list, init_token(final_val));
	free(final_val);
	s->token_in_progress = 0;
}

/**
 * @brief Creates a token for a given operator string and adds it to the list.
 * @param list Pointer to the head of the token list.
 * @param op_str The operator string (e.g., "<<", "|").
 */
static void	add_operator_as_token(t_token **list, const char *op_str)
{
	char	*op_val_dup;

	op_val_dup = ft_strdup(op_str);
	if (!op_val_dup)
		return ;
	add_token_back(list, init_token(op_val_dup));
	free(op_val_dup);
}



/**
 * @brief Parses a line of input into a list of tokens.
 * Handles operators (|, <, >, <<, >>), spaces, and quoted segments.
 * Performs variable expansion and quote stripping on tokens.
 * @param input_line The line to parse.
 * @return A pointer to the head of the token list, or NULL on failure/empty.
 */
t_token	*parse_line(const char *input_line)
{
	t_parse_state	state;
	t_token			*token_list;

	if (!input_line)
		return (NULL);
	token_list = NULL;
	state.line = input_line;
	state.i = 0;
	state.start_of_token = 0;
	state.token_in_progress = 0;
	while (state.line[state.i])
		process_line_character(&state, &token_list);
	finalize_and_add_token(&token_list, &state);
	return (token_list);
}


