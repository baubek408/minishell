/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:59:04 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/22 14:46:41 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"


typedef struct s_cmd
{
	char	*cmd;
	char	*av;
}	t_cmd;

typedef struct s_env
{
	char			*key;
	char			*val;
	struct s_env	*next;
}	t_env;

t_env	*init_env(char **line);
char	*ft_strchr(const char *s, int c);
char	*get_env_value(t_env *env, const char *key);
int		ft_strcmp(const char *s1, const char *s2);
int		cmd_checker(char *cmd);

#endif
