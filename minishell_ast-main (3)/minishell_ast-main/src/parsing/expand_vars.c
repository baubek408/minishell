/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_vars.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:12:04 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 20:54:47 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

/**
 * @brief Ensures the buffer has enough space for additional characters.
 * Reallocates the buffer if necessary, doubling its size until sufficient.
 *
 * @param buf Pointer to the buffer management structure.
 * @param additional_needed The number of additional characters space is
 * needed for.
 * @return 0 on success, 1 on memory allocation failure.
 */
static int	ensure_buffer_has_space(t_buffer_info *buf, \
	size_t additional_needed)
{
	size_t	required_total_len;
	size_t	new_size;
	char	*new_buffer;

	required_total_len = *(buf->current_len) + additional_needed + 1;
	if (required_total_len > *(buf->current_size))
	{
		new_size = *(buf->current_size);
		while (new_size < required_total_len)
		{
			if (new_size == 0)
				new_size = 256;
			else
				new_size *= 2;
		}
		new_buffer = (char *)realloc(*(buf->buffer), new_size);
		if (!new_buffer)
		{
			free(*(buf->buffer));
			*(buf->buffer) = NULL;
			return (1);
		}
		*(buf->buffer) = new_buffer;
		*(buf->current_size) = new_size;
	}
	return (0);
}

/**
 * @brief Appends a single character to the result buffer.
 * Handles buffer resizing.
 *
 * @param c The character to append.
 * @param buf_info Pointer to the buffer management structure.
 */
static void	append_char_to_buffer(char c, t_buffer_info *buf_info)
{
	if (ensure_buffer_has_space(buf_info, 1) != 0)
		return ;
	(*(buf_info->buffer))[*(buf_info->current_len)] = c;
	*(buf_info->current_len) += 1;
	(*(buf_info->buffer))[*(buf_info->current_len)] = '\0';
}

/**
 * @brief Appends a string to the result buffer.
 * Handles buffer resizing.
 *
 * @param str The string to append.
 * @param buf_info Pointer to the buffer management structure.
 */
static void	append_string_to_buffer(const char *str, t_buffer_info *buf_info)
{
	size_t	str_len;

	if (!str)
		return ;
	str_len = ft_strlen(str);
	if (str_len == 0)
		return ;
	if (ensure_buffer_has_space(buf_info, str_len) != 0)
		return ;
	ft_strcpy(*(buf_info->buffer) + *(buf_info->current_len), str);
	*(buf_info->current_len) += str_len;
}

/**
 * @brief Processes an environment variable found in the line.
 * Extracts the variable name, gets its value, and appends it to the buffer.
 *
 * @param line The input line.
 * @param line_idx_ptr Pointer to the current index in the input line.
 * @param buf_info Pointer to the buffer management structure.
 * @return 0 on success, 1 if an error occurs (e.g. during buffer op).
 */
static int	process_env_variable(const char *line, int *line_idx_ptr, \
									t_buffer_info *buf_info)
{
	int		var_start_idx;
	int		var_len;
	char	var_name_buffer[256];
	char	*env_value;

	(*line_idx_ptr)++;
	var_start_idx = *line_idx_ptr;
	while (line[*line_idx_ptr] && \
			(ft_isalnum(line[*line_idx_ptr]) || line[*line_idx_ptr] == '_'))
		(*line_idx_ptr)++;
	var_len = *line_idx_ptr - var_start_idx;
	if (var_len > 0 && var_len < 255)
	{
		ft_strncpy(var_name_buffer, line + var_start_idx, var_len);
		var_name_buffer[var_len] = '\0';
		env_value = getenv(var_name_buffer);
		if (env_value)
			append_string_to_buffer(env_value, buf_info);
	}
	else if (var_len == 0 && line[var_start_idx -1] == '$')
	{
		// Handle special cases like $? or just '$' if needed.
		// For now, if it's just '$' followed by non-var char, it's literal.
		// Or, if it's $? this part needs to be extended.
		// The original code implicitly treated $ followed by non-var as nothing.
		// To add '$' literally if it's not a var, this logic would change.
		// For now, matches original: if no var name, nothing is added.
	}
	return (0);
}

/**
 * @brief Expands environment variables in a line.
 * Variables are denoted by '$' followed by alphanumeric characters or '_'.
 * Expansion does not occur if '$' is within single quotes (ft_flag == 1).
 *
 * @param line The input line string.
 * @return A new string with variables expanded, or NULL on allocation failure.
 * The caller is responsible for freeing the returned string.
 */
char	*expand_vars(const char *line)
{
	char			*result_buffer_ptr;
	size_t			current_buffer_size;
	size_t			current_buffer_len;
	t_buffer_info	buffer_management_info;
	int				i;

	if (!line)
		return (NULL);
	current_buffer_size = 256;
	result_buffer_ptr = (char *)calloc(current_buffer_size, sizeof(char));
	if (!result_buffer_ptr)
		return (NULL);
	current_buffer_len = 0;
	buffer_management_info.buffer = &result_buffer_ptr;
	buffer_management_info.current_size = &current_buffer_size;
	buffer_management_info.current_len = &current_buffer_len;
	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && (ft_flag(line, i) != 1))
		{
			if (process_env_variable(line, &i, &buffer_management_info) != 0)
				break ;
		}
		else
		{
			append_char_to_buffer(line[i], &buffer_management_info);
			i++;
		}
		if (*(buffer_management_info.buffer) == NULL)
			return (NULL);
	}
	return (*(buffer_management_info.buffer));
}
