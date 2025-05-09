/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:50:17 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 18:51:37 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	exec_tree(t_shell *shell, t_ast *ast)
{
    int status = 0;
    int pipefd[2];          // For NODE_PIPE
    int heredoc_pipefd[2];  // For NODE_REDIR_HEREDOC
    pid_t pid1, pid2;
    int fd;
    int saved_stdout = -1;
    int saved_stdin = -1;
    char err_msg_buffer[1024]; // Buffer for custom error messages with perror
    char *line_heredoc = NULL;

    if (!ast)
        return (0);
    if (ast->type == NODE_PIPE)
    {
        if (!ast->left || !ast->right) {
            write_error("minishell: ", "invalid pipe structure", NULL);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (pipe(pipefd) == -1) {
            perror("minishell: pipe");
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        pid1 = fork();
        if (pid1 < 0) {
            perror("minishell: fork for left pipe child");
            close(pipefd[0]);
            close(pipefd[1]);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (pid1 == 0) { // Child 1 (left side of pipe)
            close(pipefd[0]);
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                perror("minishell: dup2 stdout to pipe");
                close(pipefd[1]);
                exit(EXIT_FAILURE);
            }
            close(pipefd[1]);
            exit(exec_tree(shell, ast->left));
        }
        pid2 = fork();
        if (pid2 < 0) {
            perror("minishell: fork for right pipe child");
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (pid2 == 0) { // Child 2 (right side of pipe)
            close(pipefd[1]);
            if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                perror("minishell: dup2 stdin from pipe");
                close(pipefd[0]);
                exit(EXIT_FAILURE);
            }
            close(pipefd[0]);
            exit(exec_tree(shell, ast->right));
        }
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, &status, 0);
        if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            status = 128 + WTERMSIG(status);
        }
        shell->exit_status = status;
        return status;
    }
    else if (ast->type == NODE_REDIR_OUT || ast->type == NODE_REDIR_APPEND)
    {
        if (!ast->cmd || !ast->cmd->outfile || !ast->left) {
             write_error("minishell: ", "invalid output redirection structure", NULL);
             shell->exit_status = EXIT_FAILURE;
             return (EXIT_FAILURE);
        }
        int flags = O_WRONLY | O_CREAT;
        if (ast->type == NODE_REDIR_APPEND)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
        fd = open(ast->cmd->outfile, flags, 0644);
        if (fd < 0) {
            snprintf(err_msg_buffer, sizeof(err_msg_buffer), "minishell: %s", ast->cmd->outfile);
            perror(err_msg_buffer);
            shell->exit_status = 1;
            return 1;
        }
        saved_stdout = dup(STDOUT_FILENO);
        if (saved_stdout < 0) {
            perror("minishell: dup saved_stdout");
            close(fd);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("minishell: dup2 stdout to file");
            close(fd);
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        close(fd);
        status = exec_tree(shell, ast->left);
        // fflush(stdout); // Removed as per request
        if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
             perror("minishell: dup2 restore stdout");
        }
        close(saved_stdout);
        shell->exit_status = status;
        return status;
    }
    else if (ast->type == NODE_REDIR_IN)
    {
        if (!ast->cmd || !ast->cmd->infile || !ast->left) {
             write_error("minishell: ", "invalid input redirection structure", NULL);
             shell->exit_status = EXIT_FAILURE;
             return (EXIT_FAILURE);
        }
        fd = open(ast->cmd->infile, O_RDONLY);
        if (fd < 0) {
            snprintf(err_msg_buffer, sizeof(err_msg_buffer), "minishell: %s", ast->cmd->infile);
            perror(err_msg_buffer);
            shell->exit_status = 1;
            return 1;
        }
        saved_stdin = dup(STDIN_FILENO);
        if (saved_stdin < 0) {
            perror("minishell: dup saved_stdin");
            close(fd);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("minishell: dup2 stdin from file");
            close(fd);
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        close(fd);
        status = exec_tree(shell, ast->left);
        if (dup2(saved_stdin, STDIN_FILENO) < 0) {
            perror("minishell: dup2 restore stdin");
        }
        close(saved_stdin);
        shell->exit_status = status;
        return status;
    }
    else if (ast->type == NODE_HEREDOC)
    {
        if (!ast->cmd || !ast->cmd->heredoc || !ast->left) {
            write_error("minishell: ", "invalid heredoc structure", NULL);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (pipe(heredoc_pipefd) == -1) {
            perror("minishell: pipe for heredoc");
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        // Read input for heredoc
        while (1) {
            line_heredoc = readline("> "); // Standard heredoc prompt
            if (!line_heredoc) { // Ctrl+D
                // Bash typically prints a warning here.
                char warning_msg[256];
                snprintf(warning_msg, sizeof(warning_msg),
                         "minishell: warning: here-document delimited by end-of-file (wanted `%s')",
                         ast->cmd->heredoc);
                write_error(warning_msg, NULL, NULL);
                break;
            }
            if (ft_strcmp(line_heredoc, ast->cmd->heredoc) == 0) {
                free(line_heredoc);
                break; // Delimiter found
            }
			{
				char *result = ft_strdup("");
				int i = 0;

				while (line[i])
				{
					// Обработка кавычек
					if (line[i] == '\'' || line[i] == '"')
					{
						char quote = line[i++];
						int start = i;
						while (line[i] && line[i] != quote)
							i++;
						char *segment = ft_strndup(line + start, i - start);
						result = str_append(result, segment, 0);
						free(segment);
						if (line[i] == quote)
							i++;
					}

					// Обработка переменных: $?
					else if (line[i] == '$' && line[i + 1] == '?')
					{
						char *exit_str = ft_itoa(shell->exit_status);
						result = str_append(result, exit_str, 0);
						free(exit_str);
						i += 2;
					}

					// Обработка переменных: $VAR_NAME
					else if (line[i] == '$' && (ft_isalpha(line[i + 1]) || line[i + 1] == '_'))
					{
						int start = ++i;
						while (ft_isalnum(line[i]) || line[i] == '_')
							i++;
						char *key = ft_strndup(line + start, i - start);
						char *val = get_env_value(shell->env, key);
						if (val)
							result = str_append(result, val, 0);
						free(key);
					}

					// Обычные символы
					else
					{
						char temp[2] = { line[i], 0 };
						result = str_append(result, temp, 0);
						i++;
					}
				}
				return result;
			}            write(heredoc_pipefd[1], line_heredoc, ft_strlen(line_heredoc));
            write(heredoc_pipefd[1], "\n", 1);
            free(line_heredoc);
        }
        close(heredoc_pipefd[1]); // Close write end, command will read EOF after all lines

        // Redirect stdin for the command
        saved_stdin = dup(STDIN_FILENO);
        if (saved_stdin < 0) {
            perror("minishell: dup saved_stdin for heredoc");
            close(heredoc_pipefd[0]);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        if (dup2(heredoc_pipefd[0], STDIN_FILENO) < 0) {
            perror("minishell: dup2 stdin from heredoc_pipe");
            close(heredoc_pipefd[0]);
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
            shell->exit_status = EXIT_FAILURE;
            return (EXIT_FAILURE);
        }
        close(heredoc_pipefd[0]); // Close read end after dup2

        status = exec_tree(shell, ast->left); // Execute the command

        // Restore stdin
        if (dup2(saved_stdin, STDIN_FILENO) < 0) {
            perror("minishell: dup2 restore stdin after heredoc");
        }
        close(saved_stdin);
        shell->exit_status = status;
        return status;
    }
    else if (ast->type == NODE_CMD)
    {
        if (!ast->cmd || !ast->cmd->cmd) {
            write_error("minishell: ", "invalid command in CMD node", NULL);
            shell->exit_status = 127;
            return 127;
        }
        if (buildin_checker(ast->cmd->cmd)) {
            status = find_buildin(shell, ast->cmd);
        } else {
            status = exec_external(shell, ast->cmd);
        }
        shell->exit_status = status;
        return status;
    }
    else
	{
        char unknown_type_msg[128];
        snprintf(unknown_type_msg, sizeof(unknown_type_msg), "minishell: unknown AST node type %d", ast->type);
        write_error(unknown_type_msg, NULL, NULL);
        shell->exit_status = EXIT_FAILURE;
        return EXIT_FAILURE;
    }
    return status;
}

