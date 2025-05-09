/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   musor_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:45:22 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/05/09 19:54:49 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

#include "inc/minishell.h"

// --- Helper to create a basic AST node ---
// This was implied by user's `create_cmd_node`
t_ast *create_ast_node(t_node_type type)
{
    t_ast *node = (t_ast *)malloc(sizeof(t_ast));
    if (!node) {
        perror("minishell: malloc failed for ast node");
        return NULL;
    }
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->cmd = NULL; // Initialize cmd part to NULL
    return node;
}



void print_tokens(t_token *tokens)
{
	char *token_type_str[] = {
		"TOKEN_PIPE",
		"TOKEN_REDIR_IN",
		"TOKEN_REDIR_OUT",
		"TOKEN_REDIR_APPEND",
		"TOKEN_HEREDOC",
		"TOKEN_WORD",
		"TOKEN_SEMI"
	};

	printf("Tokens list:\n");
	while (tokens)
	{
		if (tokens->type >= 0 && tokens->type <= TOKEN_SEMI)
			printf("Value: %-10s | Type: %s\n", tokens->value, token_type_str[tokens->type]);
		else
			printf("Value: %-10s | Type: UNKNOWN\n", tokens->value);
		tokens = tokens->next;
	}
}
void free_argv_array(char **argv) {
    if (!argv) return;
    for (int i = 0; argv[i]; i++) {
        free(argv[i]);
    }
    free(argv);
}

t_ast *create_cmd_node(char *cmd_name_allocated, char **args_allocated)
{
    t_ast *node = create_ast_node(NODE_CMD);
    if (!node) {
        // Caller is responsible for freeing cmd_name_allocated and args_allocated
        // if create_ast_node itself fails.
        return NULL;
    }
    node->cmd = (t_cmd *)malloc(sizeof(t_cmd));
    if (!node->cmd) {
        perror("minishell: malloc failed for t_cmd");
        free(node); // Free the AST node allocated by create_ast_node
        // Caller is responsible for freeing cmd_name_allocated and args_allocated
        return NULL;
    }
    node->cmd->cmd = cmd_name_allocated; // Takes ownership
    node->cmd->args = args_allocated;   // Takes ownership
    node->cmd->outfile = NULL;
    node->cmd->append = 0;
    node->cmd->infile = NULL;
    node->cmd->heredoc = NULL;
    return node;
}

/**
 * @brief Creates a NODE_CMD AST node from a range of tokens.
 * Collects all TOKEN_WORDs into an argv-style array.
 * All strings in the returned node's t_cmd (cmd and args) are fresh duplicates.
 *
 * @param start The first token in the segment for the command.
 * @param end The token marking the end of the segment (exclusive).
 * @return t_ast* A new AST node of type NODE_CMD, or NULL on failure or if no words.
 */
t_ast *create_cmd_node_from_tokens(t_token *start, t_token *end) {
    t_token *current;
    char    **args_array = NULL;
    char    *cmd_name_val_from_args0 = NULL;
    char    *cmd_name_final_dup = NULL;
    int     word_count = 0;
    int     i = 0;
    t_ast   *new_node = NULL;

    if (!start || start == end)
        return (NULL);

    current = start;
    while (current != NULL && current != end) {
        if (current->type == TOKEN_WORD)
            word_count++;
        current = current->next;
    }
    if (word_count == 0)
        return (NULL);

    args_array = (char **)malloc(sizeof(char *) * (word_count + 1));
    if (!args_array) {
        perror("minishell: malloc for args_array failed");
        return (NULL);
    }

    current = start;
    i = 0;
    while (current != NULL && current != end && i < word_count) {
        if (current->type == TOKEN_WORD) {
            args_array[i] = ft_strdup(current->value);
            if (!args_array[i]) {
                perror("minishell: ft_strdup for arg failed");
                for (int k = 0; k < i; k++) free(args_array[k]);
                free(args_array);
                return (NULL);
            }
            if (i == 0)
                cmd_name_val_from_args0 = args_array[i];
            i++;
        }
        current = current->next;
    }
    args_array[i] = NULL;

    if (!cmd_name_val_from_args0) { // Should not happen if word_count > 0
        fprintf(stderr, "minishell: internal parser error - no command name found\n");
        free_argv_array(args_array); // Use helper to free args_array and its contents
        return NULL;
    }

    // Create a separate duplicate for node->cmd->cmd
    cmd_name_final_dup = ft_strdup(cmd_name_val_from_args0);
    if (!cmd_name_final_dup) {
        perror("minishell: ft_strdup for cmd_name_final_dup failed");
        free_argv_array(args_array);
        return NULL;
    }

    new_node = create_cmd_node(cmd_name_final_dup, args_array);
    if (!new_node) {
        // If create_cmd_node failed, it didn't take ownership, so we free them.
        free(cmd_name_final_dup);
        free_argv_array(args_array);
        return NULL;
    }
    return new_node;
}


/**
 * @brief Builds an Abstract Syntax Tree (AST) from a sequence of tokens.
 * Handles pipes and redirections.
 *
 * @param start The first token in the current segment to parse.
 * @param end The token marking the end of the current segment (exclusive).
 * If NULL, parse until the end of the list starting from 'start'.
 * @return t_ast* The root of the constructed AST for this segment, or NULL on error.
 */
t_ast *build_ast_from_tokens(t_token *start, t_token *end) {
    t_token *current;
    t_ast   *node = NULL;

    if (!start || start == end)
        return (NULL);

    // 1. Parse for Pipes (lowest precedence, usually left-associative)
    // To handle left-associativity correctly, find the *last* pipe in the segment.
    t_token *pipe_token = NULL;
    current = start;
    while (current != NULL && current != end) {
        if (current->type == TOKEN_PIPE) {
            pipe_token = current; // Keep track of the last pipe found
        }
        current = current->next;
    }

    if (pipe_token != NULL) {
        node = create_ast_node(NODE_PIPE);
        if (!node) return (NULL);
        node->left = build_ast_from_tokens(start, pipe_token);
        node->right = build_ast_from_tokens(pipe_token->next, end);
        if (!node->left || !node->right) {
            if (!node->left && pipe_token != start) fprintf(stderr, "minishell: syntax error near `|'\n");
            else if (!node->right && pipe_token->next != end) fprintf(stderr, "minishell: syntax error near `|'\n");
            else if (!node->left && !node->right) fprintf(stderr, "minishell: syntax error near `|' (empty sides)\n");
            // Proper cleanup would involve a free_ast function
            if(node->left) free(node->left); // Simplistic cleanup for now
            if(node->right) free(node->right);
            free(node);
            return (NULL);
        }
        return (node);
    }

    // 2. Parse for Redirections (find the *last* redirection in the segment)
    // This makes redirections bind more tightly to simple commands than pipes.
    t_token *last_redir_op = NULL;
    t_token *last_redir_file_or_delim = NULL;

    current = start;
    while(current != NULL && current != end) {
        if (current->type == TOKEN_REDIR_IN || current->type == TOKEN_REDIR_OUT ||
            current->type == TOKEN_REDIR_APPEND || current->type == TOKEN_HEREDOC) {
            if (current->next != NULL && current->next != end && current->next->type == TOKEN_WORD) {
                last_redir_op = current;
                last_redir_file_or_delim = current->next;
            } else {
                fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n", current->value);
                return NULL; // Missing filename/delimiter
            }
        }
        current = current->next;
    }

    if (last_redir_op != NULL) { // A redirection was found
        t_node_type redir_node_type;
        if (last_redir_op->type == TOKEN_REDIR_IN) redir_node_type = NODE_REDIR_IN;
        else if (last_redir_op->type == TOKEN_REDIR_OUT) redir_node_type = NODE_REDIR_OUT;
        else if (last_redir_op->type == TOKEN_REDIR_APPEND) redir_node_type = NODE_REDIR_APPEND;
        else if (last_redir_op->type == TOKEN_HEREDOC) redir_node_type = NODE_HEREDOC;
        else return NULL; // Should not happen

        node = create_ast_node(redir_node_type);
        if (!node) return NULL;
        node->cmd = (t_cmd *)malloc(sizeof(t_cmd));
        if (!node->cmd) { free(node); perror("minishell: malloc for redir t_cmd"); return NULL; }

        node->cmd->cmd = NULL; node->cmd->args = NULL; node->cmd->outfile = NULL;
        node->cmd->infile = NULL; node->cmd->heredoc = NULL; node->cmd->append = 0;

        char *temp_val_dup = ft_strdup(last_redir_file_or_delim->value);
        if(!temp_val_dup){
            perror("minishell: ft_strdup for redir file/delim failed");
            free(node->cmd); free(node); return NULL;
        }

        if (redir_node_type == NODE_REDIR_IN) node->cmd->infile = temp_val_dup;
        else if (redir_node_type == NODE_REDIR_OUT) node->cmd->outfile = temp_val_dup;
        else if (redir_node_type == NODE_REDIR_APPEND) {
            node->cmd->outfile = temp_val_dup;
            node->cmd->append = 1;
        } else if (redir_node_type == NODE_HEREDOC) node->cmd->heredoc = temp_val_dup;

        // Construct a new token list for the recursive call, excluding the redirection part.
        // This is the most complex part to get right for all cases.
        // Approach: Parse tokens before the redir_op, and tokens after redir_file_or_delim separately,
        // then combine them if necessary, or decide which part is the "command".
        // For `cmd > file` or `> file cmd`, the `cmd` part is what `node->left` should be.

        // Create a temporary list of tokens that excludes the processed redirection
        t_token *temp_token_list_head = NULL, *temp_token_list_tail = NULL;
        t_token *iter = start;
        while(iter != NULL && iter != end){
            if(iter == last_redir_op || iter == last_redir_file_or_delim){
                iter = iter->next; // Skip redir_op
                if (iter == last_redir_file_or_delim) iter = iter->next; // Skip file/delim
                continue;
            }
            // Add token to temp list (needs a function to duplicate token value for safety,
            // or ensure build_ast_from_tokens doesn't modify original token values)
            // For simplicity, let's assume we can build from sub-segments.
            // This part needs careful implementation of sub-list creation or modification.
            // The provided code below is a simplification.
            // A robust solution would create a new list or adjust pointers carefully.
            // For now, we parse what's before the redir op and what's after its argument.
            iter = iter->next;
        }

        // Simplified logic: parse everything that is NOT the last redirection found.
        // This requires creating a new token list without the redirection tokens.
        // This is non-trivial. A common approach is to parse the command part first.
        // If `cmd > file`, `cmd` is `build_ast_from_tokens(start, last_redir_op)`.
        // If `> file cmd`, `cmd` is `build_ast_from_tokens(last_redir_file_or_delim->next, end)`.

        // Let's try to determine the command part based on position of redirection
        if (last_redir_op == start) { // e.g. `> outfile cmd`
            node->left = build_ast_from_tokens(last_redir_file_or_delim->next, end);
        } else { // e.g. `cmd > outfile` or `cmd1 > outfile cmd2`
            // For `cmd1 > outfile cmd2`, `last_redir_op` would be for `cmd2` if it has one,
            // or for `cmd1` if `cmd2` has no redir.
            // This logic of finding LAST redirection means it will be the top node.
            // The `left` child should be the result of parsing everything else.
            // This requires building a temporary list without the redir and its arg.
            // This is where a more advanced parsing technique (like shunting-yard or
            // recursive descent with explicit precedence) is usually needed.

            // Simpler approach for now: assume redir applies to a simple command.
            // The part *before* last_redir_op is parsed. If there are tokens *after*
            // last_redir_file_or_delim, they are currently ignored by this structure for node->left.
            node->left = build_ast_from_tokens(start, last_redir_op);
        }

        // If node->left is NULL and it's not just a case like "> file", it might be an error.
        if (!node->left && !(last_redir_op == start && (last_redir_file_or_delim->next == end || last_redir_file_or_delim->next == NULL))) {
             // This means tokens before redir didn't form a command, or tokens after (if redir was first) didn't.
             // Proper error: "syntax error near redirection" or "missing command"
             // For now, let's allow node->left to be NULL if it's like "> outfile" (creates empty file)
             // but if there were other tokens that should have formed a command, it's an error.
             // This part needs more robust error checking.
        }
        return (node);
    }

    // 3. If no pipes and no redirections were processed as the main operator for this segment,
    //    then it's a simple command.
    return create_cmd_node_from_tokens(start, end);
}

void print_cmd(t_cmd *cmd)
{
	if (!cmd)
	{
		printf("(null cmd)\n");
		return;
	}
	printf("Command: %s\n", cmd->cmd);
	printf("Args: ");
	int i = 0;
	while (cmd->args && cmd->args[i])
	{
		printf("[%s] ", cmd->args[i]);
		i++;
	}
	printf("\n");

	if (cmd->outfile)
		printf("Outfile: %s (append=%d)\n", cmd->outfile, cmd->append);

	if (cmd->infile)
		printf("Infile: %s\n", cmd->infile);

	if (cmd->heredoc)
		printf("Heredoc: %s\n", cmd->heredoc);
}

void print_ast(t_ast *node, int level)
{
	if (!node)
		return;

	for (int i = 0; i < level; i++)
		printf("    ");

	if (node->type == NODE_PIPE)
		printf("[PIPE]\n");
	else if (node->type == NODE_REDIR_OUT)
		printf("[REDIR_OUT]\n");
	else if (node->type == NODE_CMD)
	{
		printf("[CMD]\n");
		for (int i = 0; i < level + 1; i++)
			printf("    ");
		print_cmd(node->cmd);
	}

	// Рекурсивно печатаем левое и правое поддеревья
	print_ast(node->left, level + 1);
	print_ast(node->right, level + 1);
}


void write_error(const char *s1, const char *s2, const char *s3)
{
    if (s1)
        write(STDERR_FILENO, s1, ft_strlen(s1));
    if (s2)
        write(STDERR_FILENO, s2, ft_strlen(s2));
    if (s3)
        write(STDERR_FILENO, s3, ft_strlen(s3));
    write(STDERR_FILENO, "\n", 1);
}



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
    else {
        char unknown_type_msg[128];
        snprintf(unknown_type_msg, sizeof(unknown_type_msg), "minishell: unknown AST node type %d", ast->type);
        write_error(unknown_type_msg, NULL, NULL);
        shell->exit_status = EXIT_FAILURE;
        return EXIT_FAILURE;
    }
    return status;
}
