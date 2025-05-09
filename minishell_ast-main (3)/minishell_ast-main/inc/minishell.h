/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 17:46:06 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 20:52:39 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <sys/stat.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "inc/libft/libft.h"

typedef enum e_token_type
{
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_WORD,
	TOKEN_SEMI
}	t_token_type;


typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_REDIR_OUT,
	NODE_REDIR_IN,
	NODE_REDIR_APPEND,
	NODE_HEREDOC
}	t_node_type;

typedef struct s_cmd
{
	char	*cmd;
	char	**args;
	char	*outfile;
	int		append;
	char	*infile;
	char	*heredoc;
}	t_cmd;

typedef struct s_ast
{
	t_node_type		type;
	struct s_ast	*left;
	struct s_ast	*right;
	t_cmd			*cmd;
}	t_ast;

typedef struct s_env
{
	char			*key;
	char			*val;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	t_token	*tokens;
	t_ast	*ast;
	t_env	*env;
	int		exit_status;
	// char  *input_line;
	// int    is_running;
}	t_shell;

typedef struct s_parse_state
{
	const char	*line;
	int			i;
	int			start_of_token;
	int			token_in_progress;
}	t_parse_state;

typedef struct s_str_copy_params
{
	const char	*s1;
	const char	*s2;
	size_t		len1;
	int			add_newline;
}				t_str_copy_params;

typedef struct s_buffer_info
{
	char	**buffer;
	size_t	*current_size;
	size_t	*current_len;
}				t_buffer_info;

t_token 	*parse_line(char *line);
t_token 	*init_token(const char *value);
t_env 		*init_env(char **line);

void		free_token_list(t_token *token);
void		free_ast(t_ast *node);
void 		add_token_back(t_token **head, t_token *new);

char 		*get_env_value(t_env *env, const char *key);

int			ft_flag(const char *line, int pos);
int 		ft_strcmp(const char *s1, const char *s2);
int 		buildin_checker(char *cmd);

//	env
void 		add_env_node(t_env **env_list, const char *key, const char *value);
t_env 		*new_env_node(const char *key, const char *value);
t_env 		*find_env_key(t_env *env, const char *key);
void		ft_free_env(t_env *env);


//	standart
void		ft_putstr(const char *s);
char 		*str_append(const char *s1, const char *s2, int add_newline);
char		*ft_strdup(const char *s);
char 		*ft_strndup(const char *s, size_t n);
char		*ft_strchr(const char *s, int c);
char 		*read_q(char *line);
void		ft_putstr_fd(const char *s, int fd);

//	build-in
int 		find_buildin(t_shell *shell, t_cmd *cmd);
int			ft_env(t_shell *shell);
int 		ft_echo(t_shell *shell,t_cmd *cmd);
int			ft_pwd();
void 		print_export(t_env *env);
int			ft_export(t_shell *shell, t_cmd *cmd);
void 		cleanup_shell(t_shell *shell);
int 		ft_exit(t_shell *shell);
int			ft_unset(t_shell *shell, t_cmd *cmd);
void 		change_env_value(t_env **env_list_ptr, const char *key, const char *value);
int			ft_cd(t_shell *shell, t_cmd *cmd);

//	cmds
int			exec_external(t_shell *shell, t_cmd *cmd);
char 		*find_command_path(char *cmd, t_env *envp);


int is_valid_env_key(const char *key);
int set_env_variable(t_env **env_list_ptr, const char *key, const char *value);
int	ft_strlen(const char *str);

#endif
