/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:50:17 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 19:02:15 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

/**
 * @brief Main execution function for the AST.
 * Dispatches to specific handlers based on the AST node type.
 * @param shell The shell structure.
 * @param ast The current AST node to execute.
 * @return The exit status of the executed command or operation.
 */

int	exec_tree(t_shell *shell, t_ast *ast)
{
	int		status;

	status = 0;
	if (!ast)
		return (0);
	if (ast->type == NODE_PIPE)
		status = exec_pipe_node(shell, ast);
	else if (ast->type == NODE_REDIR_OUT || ast->type == NODE_REDIR_APPEND
		|| ast->type == NODE_REDIR_IN)
		status = exec_redir_node(shell, ast);
	else if (ast->type == NODE_HEREDOC)
		status = exec_heredoc_node(shell, ast);
	else if (ast->type == NODE_CMD)
		status = exec_cmd_node(shell, ast);
	else
	{
		write_error("unknown_type_msg", NULL, NULL);
		status = EXIT_FAILURE;
	}
	shell->exit_status = status;
	return (status);
}

