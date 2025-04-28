/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:59:04 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/28 17:36:33 by bmynbyae         ###   ########.fr       */
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

typedef struct s_env_node
{
	char				*key;
	char				*value;
	struct s_env_node	*left;
	struct s_env_node	*right;
}	t_env_node;

typedef enum e_token_type
{
	TOKEN_WORD,         // For command names, arguments, filenames
	TOKEN_PIPE,         // |
	TOKEN_REDIR_IN,     // <
	TOKEN_REDIR_OUT,    // >
	TOKEN_REDIR_APPEND, // >>
	TOKEN_REDIR_HEREDOC,// <<
	TOKEN_NEWLINE,      // Optional: If you want to represent end of line
	TOKEN_ERROR         // To signify a lexing error (e.g., unclosed quote)
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;// Allocated string (e.g., "ls", ">", "my file.txt")
	struct s_token	*next; // Pointer to the next token in the sequence
}	t_token;

//utils//

void		cleanup_shell(t_env_node *env_root);
void		shell_loop(t_env_node *env_root);
t_env_node	*init_shell(char **envp);
void		free_env_bst(t_env_node *root);
void		print_env_bst(t_env_node *node);

//env//

t_env_node	*new_bst_node(const char *key, const char *value);
t_env_node	*insert_or_update_bst_node(t_env_node *node, const char *key, \
	const char *value);
void		add_env_bst(t_env_node **root_ptr, const char *key, \
	const char *value);
t_env_node	*init_env_bst(char **environ);

//env_2//

char		*get_env_value(t_env_node *root, const char *key);

//cmd_cheker//

int			cmd_checker(char *cmd);




#endif
