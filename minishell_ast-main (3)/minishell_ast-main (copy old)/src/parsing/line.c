/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   musor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:23:48 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 18:45:20 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

char	*preprocess_line(const char *line, t_shell *shell)
{
	char	*result_str;
	int		i;
	int		status;

	result_str = ft_strdup("");
	if (!result_str)
		return (NULL);
	i = 0;
	status = 0;
	while (line[i] && status == 0)
	{
		if (line[i] == '\'' || line[i] == '"')
			status = process_quoted_segment(line, &i, &result_str);
		else if (line[i] == '$' && line[i + 1] == '?')
			status = process_exit_status_expansion(&i, &result_str, shell);
		else if (line[i] == '$' && \
				(ft_isalpha(line[i + 1]) || line[i + 1] == '_'))
			status = process_variable_expansion(line, &i, &result_str, shell);
		else
			status = process_regular_char(line, &i, &result_str);
	}
	if (status != 0)
		return (free(result_str), NULL);
	return (result_str);
}
