/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_preprocess.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:34:31 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 18:44:35 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

static int	append_to_result_str(char **main_str, char *add_str, int free_add)
{
	char	*new_str_ptr;
	char	*old_main_ptr;

	if (!add_str)
		return (0);
	old_main_ptr = *main_str;
	new_str_ptr = str_append(old_main_ptr, add_str, 0);
	if (free_add)
		free(add_str);
	if (!new_str_ptr)
		return (1);
	free(old_main_ptr);
	*main_str = new_str_ptr;
	return (0);
}

int	process_quoted_segment(const char *l, int *i, char **r_str)
{
	int		start;
	char	quote_char;
	char	*segment;

	quote_char = l[*i];
	(*i)++;
	start = *i;
	while (l[*i] && l[*i] != quote_char)
		(*i)++;
	segment = ft_strndup(l + start, *i - start);
	if (!segment)
		return (1);
	if (append_to_result_str(r_str, segment, 1))
		return (1);
	if (l[*i] == quote_char)
		(*i)++;
	return (0);
}

int	process_exit_status_expansion(int *i, char **r_str, t_shell *sh)
{
	char	*exit_s;

	exit_s = ft_itoa(sh->exit_status);
	if (!exit_s)
		return (1);
	if (append_to_result_str(r_str, exit_s, 1))
		return (1);
	*i += 2;
	return (0);
}

int	process_variable_expansion(const char *l, int *i, char **r_str, \
	t_shell *sh)
{
	int		key_start;
	char	*key;
	char	*val;

	(*i)++;
	key_start = *i;
	while (ft_isalnum(l[*i]) || l[*i] == '_')
		(*i)++;
	key = ft_strndup(l + key_start, *i - key_start);
	if (!key)
		return (1);
	val = get_env_value(sh->env, key);
	if (val)
	{
		if (append_to_result_str(r_str, val, 0))
		{
			free(key);
			return (1);
		}
	}
	free(key);
	return (0);
}

int	process_regular_char(const char *l, int *i, char **r_str)
{
	char	temp_char_str[2];

	temp_char_str[0] = l[*i];
	temp_char_str[1] = '\0';
	if (append_to_result_str(r_str, temp_char_str, 0))
		return (1);
	(*i)++;
	return (0);
}


