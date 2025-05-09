/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:45:47 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 20:46:42 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"


/**
 * @brief Copies s1 and s2 into the destination string.
 * Optionally adds a newline character between s1 and s2.
 *
 * @param dest The destination buffer where strings will be copied.
 * @param params Pointer to a structure containing s1, s2, len1, and add_newline.
 */
static void	perform_string_copy(char *dest, t_str_copy_params *params)
{
	ft_strcpy(dest, params->s1);
	if (params->add_newline)
	{
		dest[params->len1] = '\n';
		ft_strcpy(dest + params->len1 + 1, params->s2);
	}
	else
		ft_strcpy(dest + params->len1, params->s2);
}

/**
 * @brief Concatenates two strings, s1 and s2, into a new string.
 * Optionally adds a newline character between s1 and s2.
 * The caller is responsible for freeing the returned string.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param add_newline If non-zero, a newline char is added between s1 and s2.
 * @return A pointer to the newly allocated concatenated string,
 * or NULL if memory allocation fails or input strings are NULL.
 */
char	*str_append(const char *s1, const char *s2, int add_newline)
{
	char				*result;
	size_t				len1_val;
	size_t				len2;
	size_t				total_len;
	t_str_copy_params	copy_params;

	if (!s1 || !s2)
		return (NULL);
	len1_val = ft_strlen(s1);
	len2 = ft_strlen(s2);
	total_len = len1_val + len2 + 1;
	if (add_newline)
		total_len += 1;
	result = (char *)malloc(total_len);
	if (!result)
		return (NULL);
	copy_params.s1 = s1;
	copy_params.s2 = s2;
	copy_params.len1 = len1_val;
	copy_params.add_newline = add_newline;
	perform_string_copy(result, &copy_params);
	return (result);
}

char	*read_q(char *line)
{
	char	*next_line;
	char	*combined;
	char	*tmp;

	combined = ft_strdup(line);
	if (!combined)
		return (NULL);
	while (ft_flag(combined, ft_strlen(combined)))
	{
		next_line = readline("quotes$ ");
		if (!next_line)
			break ;
		tmp = str_append(combined, next_line, 1);
		free(combined);
		free(next_line);
		combined = tmp;
		if (!combined)
			break ;
	}
	return (combined);
}

/// -------------------------------------------------------------------------------------------------------------- ///
/// ------------------------------------------------quotes-------------------------------------------------------- ///
/// -------------------------------------------------------------------------------------------------------------- ///

int	quotes_checker_len(char *line)
{
	int		i;
	char	quotes;
	int		len;

	i = 0;
	quotes = 0;
	len = 0;
	while (line[i])
	{
		if ((line[i] == '\'' || line[i] == '\"') && ft_flag(line, i) == 0)
			quotes = line[i];
		else if (quotes == line[i] && ft_flag(line, i) != 0)
			quotes = 0;
		else
			len++;
		i++;
	}
	return (len);
}

char	*strip_quotes(char *line)
{
	char	quotes;
	size_t	l;
	int		old;
	int		new;
	char	*dup;

	l = quotes_checker_len(line);
	quotes = 0;
	old = 0;
	new = 0;
	dup = malloc(l + 1);
	if (!dup)
		return (NULL);
	while (line[old] && new < l)
	{
		if ((line[old] == '\'' || line[old] == '\"') && ft_flag(line, old) == 0)
			quotes = line[old];
		else if (quotes == line[old] && ft_flag(line, old) != 0)
			quotes = 0;
		else
		{
			dup[new] = line[old];
			new++;
		}
		old++;
	}
	dup[new] = '\0';
	return (dup);
}
