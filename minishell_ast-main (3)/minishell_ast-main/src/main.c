/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:04:26 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 20:57:25 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/minishell.h"

/*
 * @brief Initializes the shell structure.
 * @param shell The shell structure to initialize.
 * @param envp The environment variables from main.
 */
static void	init_shell_data(t_shell *shell, char **envp)
{
	shell->tokens = NULL;
	shell->ast = NULL;
	shell->env = init_env(envp);
	shell->exit_status = 0;
}

/*
 * @brief Processes a single command line: parsing, AST building, execution.
 * @param shell The shell structure.
 * @param line The command line string to process (already preprocessed).
 */

static void	process_command_line(t_shell *shell, char *line)
{
	if (!line || !*line)
		return ;

	shell->tokens = parse_line(line);
	if (!shell->tokens)
	{
		shell->exit_status = 1;
		return ;
	}
	label_tokens(shell->tokens);
	shell->ast = build_ast_from_tokens(shell->tokens, NULL);
	if (shell->ast)
		shell->exit_status = exec_tree(shell, shell->ast);
	else
		shell->exit_status = 2;
	free_command_resources(shell);
}
/*
 * @brief Handles potentially multi-line input if quotes are open.
 * Takes ownership of current_line if it's extended (frees it).
 * @param current_line The initial line read from readline.
 * @return The complete line (possibly new, possibly original),
 * or NULL on error/EOF in read_q.
 */

static char	*get_complete_line(char *current_line)
{
	char	*extended_line;

	if (!current_line)
		return (NULL);
	if (ft_flag(current_line, ft_strlen(current_line)))
	{
		extended_line = read_q(current_line); //rewrite
		free(current_line);
		return (extended_line);
	}
	return (current_line);
}

/*
 * @brief Main loop of the minishell.
 * @param shell The shell structure.
 */

static void	minishell_loop(t_shell *shell)
{
	char	*line;
	char	*processed_line;

	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (*line == '\0')
		{
			free(line);
			continue ;
		}
		processed_line = get_complete_line(line);
		if (processed_line)
		{
			add_history(processed_line);
			process_command_line(shell, processed_line);
			free(processed_line);
		}
	}
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell_data;

	(void)ac;
	(void)av;
	init_shell_data(&shell_data, envp);
	minishell_loop(&shell_data);
	if (shell_data.env)
		free_env_list(shell_data.env);
	return (shell_data.exit_status);
}
