#include "minishell.h"

int is_valid_env_key(const char *key)
{
    if (!key || *key == '\0') // Cannot be NULL or empty
        return (0);
    if (!ft_isalpha(*key) && *key != '_') // Must start with letter or underscore
        return (0);
    key++;
    while (*key)
    {
        if (!ft_isalnum(*key) && *key != '_') // Subsequent chars must be alphanumeric or _
            return (0);
        key++;
    }
    return (1);
}

// --- Helper Function: Set/Update an environment variable ---
/**
 * @brief Sets or updates an environment variable in the linked list.
 * If the key exists, its value is updated. Otherwise, a new variable is added.
 * This function duplicates the key and value strings it stores.
 *
 * @param env_list_ptr Pointer to the head of the environment list.
 * @param key The key of the environment variable.
 * @param value The value of the environment variable.
 * @return int 0 on success, 1 on malloc failure.
 */
int set_env_variable(t_env **env_list_ptr, const char *key, const char *value)
{
    t_env *current;
    t_env *prev;
    char  *new_value_dup;
    char  *new_key_dup;

    current = *env_list_ptr;
    prev = NULL;
    // Search for existing key
    while (current != NULL)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            new_value_dup = ft_strdup(value);
            if (!new_value_dup) {
                perror("minishell: export: malloc");
                return (1); // Malloc failure
            }
            free(current->val);
            current->val = new_value_dup;
            return (0); // Success
        }
        prev = current;
        current = current->next;
    }

    // Key not found, add new variable
    new_key_dup = ft_strdup(key);
    if (!new_key_dup) { perror("minishell: export: malloc"); return (1); }
    new_value_dup = ft_strdup(value);
    if (!new_value_dup) {
        perror("minishell: export: malloc");
        free(new_key_dup);
        return (1);
    }
    current = (t_env *)malloc(sizeof(t_env)); // New node
    if (!current) {
        perror("minishell: export: malloc");
        free(new_key_dup);
        free(new_value_dup);
        return (1);
    }
    current->key = new_key_dup;
    current->val = new_value_dup;
    current->next = NULL;
    if (prev == NULL) // List was empty or adding to head
        *env_list_ptr = current;
    else
        prev->next = current;
    return (0); // Success
}

static int is_n_option(const char *arg)
{
    int j;

    if (arg == NULL || arg[0] != '-' || arg[1] != 'n')
        return (0); // Должен начинаться с "-n"

    j = 2; // Начинаем проверку с символа после "-n"
    while (arg[j] != '\0')
    {
        if (arg[j] != 'n')
            return (0); // Если после "-n" есть что-то кроме 'n', это не флаг
        j++;
    }
    return (1); // Это валидный флаг "-n..."
}

int ft_echo(t_shell *shell, t_cmd *cmd)
{
    int i;
    int print_newline;

    (void)shell; // shell может не использоваться напрямую в логике echo,
                 // но передается для единообразия с другими встроенными командами.

    i = 1; // Начинаем проверку аргументов с cmd->args[1]
    print_newline = 1; // По умолчанию: печатать символ новой строки в конце

    // Проверяем опцию -n... как *первый* аргумент
    if (cmd->args[1] != NULL && is_n_option(cmd->args[1]))
    {
        print_newline = 0; // Подавить новую строку
        i++;               // Начать печать со следующего аргумента (cmd->args[2])
    }
    // Печать аргументов
    while (cmd->args[i] != NULL)
    {
        // Используем write для строгого соответствия разрешенным функциям
        write(STDOUT_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
        // printf("%s", cmd->args[i]); // Альтернатива с использованием printf

        if (cmd->args[i + 1] != NULL) // Если есть еще один аргумент, печатаем пробел
        {
            write(STDOUT_FILENO, " ", 1);
            // printf(" ");
        }
        i++;
    }

    // Печать завершающего символа новой строки, если опция -n... не использовалась
    if (print_newline)
    {
        write(STDOUT_FILENO, "\n", 1);
        // printf("\n");
    }

    return (0); // Встроенная команда echo обычно возвращает 0 (успех)
}

int ft_env(t_shell *shell) // Изменен тип возвращаемого значения на int
{
	t_env *current_env;

	current_env = shell->env; // Получаем начало списка переменных окружения
	while (current_env != NULL)
	{
		// Печатаем переменную только если у нее есть значение (val не NULL).
		// Если val это пустая строка "", то "KEY=" будет напечатано.
		// Если val это NULL, то переменная не печатается.
		if (current_env->val != NULL)
		{
			printf("%s=%s\n", current_env->key, current_env->val);
		}
		current_env = current_env->next; // Переходим к следующей переменной
	}
	return (0); // Команда env обычно завершается успешно
}

/// FOR CD

int ft_pwd(t_shell *shell, t_cmd *cmd) // Добавлены аргументы для единообразия
{
    char *cwd_val; // Используем указатель для динамического выделения

    (void)cmd; // Если аргументы команды не используются для pwd

    cwd_val = getcwd(NULL, 0); // getcwd выделит память нужного размера
    if (cwd_val != NULL)
    {
        printf("%s\n", cwd_val);
        free(cwd_val); // Освобождаем память, выделенную getcwd
        if (shell) // Устанавливаем статус, если shell передан
            shell->exit_status = 0;
        return (0); // 0 - успех
    }
    else
    {
        perror("minishell: pwd"); // Более стандартное сообщение об ошибке
        if (shell) // Устанавливаем статус, если shell передан
            shell->exit_status = 1;
        return (1); // 1 - ошибка
    }
}

void print_export(t_env *env1)
{
	t_env *env = env1;
	while (env)
	{
		printf("declare -x %s", env->key);
		if (env->val)
			printf("=\"%s\"", env->val);
		printf("\n");
		env = env->next;
	}
}

int ft_export(t_shell *shell, t_cmd *cmd)
{
    int     i;
    char    *key_str;
    char    *value_str;
    char    *equal_sign;
    int     overall_status;

    i = 1;
    overall_status = 0; // Assume success initially

    // Case 1: `export` with no arguments
    if (cmd->args[1] == NULL)
    {
        print_export(shell->env);
        return (0); // Success
    }

    // Case 2: `export KEY=VALUE [KEY2=VALUE2 ...]`
    while (cmd->args[i] != NULL)
    {
        equal_sign = ft_strchr(cmd->args[i], '=');
        key_str = NULL;
        value_str = NULL;

        if (equal_sign != NULL) // Assignment form: KEY=VALUE
        {
            int key_len = equal_sign - cmd->args[i];
            if (key_len == 0) // Empty key: "=VALUE"
            {
                fprintf(stderr, "minishell: export: `%s': not a valid identifier\n", cmd->args[i]);
                overall_status = 1;
            }
            else
            {
                key_str = ft_strndup(cmd->args[i], key_len);
                if (!key_str) { perror("minishell: export: malloc"); overall_status = 1; i++; continue; }

                if (!is_valid_env_key(key_str))
                {
                    fprintf(stderr, "minishell: export: `%s': not a valid identifier\n", key_str);
                    overall_status = 1;
                }
                else
                {
                    value_str = ft_strdup(equal_sign + 1);
                    if (!value_str) { perror("minishell: export: malloc"); overall_status = 1; }
                    else
                    {
                        if (set_env_variable(&(shell->env), key_str, value_str) != 0)
                            overall_status = 1; // Propagate malloc error from set_env
                        free(value_str); // set_env_variable duplicated it
                    }
                }
                free(key_str);
            }
        }
        else // No '=' found: `export KEY` form
        {
            // For Minishell, if `export VAR` is encountered:
            // If VAR is not a valid identifier, it's an error.
            // If it IS a valid identifier, it's a no-op for environment variables
            // (as we don't distinguish shell vars to be marked for export).
            if (!is_valid_env_key(cmd->args[i]))
            {
                fprintf(stderr, "minishell: export: `%s': not a valid identifier\n", cmd->args[i]);
                overall_status = 1;
            }
            // Else: valid identifier, no '=', no operation on environment.
        }
        i++;
    }
    return (overall_status);
}

void cleanup_shell(t_shell *shell)
{
    free_token_list(shell->tokens);
    free_ast(shell->ast);
    ft_free_env(shell->env);
}

int ft_exit(t_shell *shell)
{
	cleanup_shell(shell);
    exit(shell->exit_status);
}

static int remove_env_variable(t_env **env_list_ptr, const char *key_to_unset)
{
    t_env *current;
    t_env *prev;

    // Validate the key first
    if (!is_valid_env_key(key_to_unset))
    {
        fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", key_to_unset);
        return (1); // Error status for invalid identifier
    }

    current = *env_list_ptr;
    prev = NULL;

    while (current != NULL)
    {
        if (ft_strcmp(current->key, key_to_unset) == 0)
        {
            if (prev == NULL) // Removing the head node
            {
                *env_list_ptr = current->next;
            }
            else // Removing a node in the middle or at the end
            {
                prev->next = current->next;
            }
            free(current->key);
            free(current->val);
            free(current);
            return (0); // Successfully removed (or found and removed)
        }
        prev = current;
        current = current->next;
    }
    return (0); // Key not found is not an error for unset
}

/**
 * @brief Implements the 'unset' built-in command.
 * Removes specified environment variables.
 * Handles multiple arguments. Validates variable names.
 *
 * @param shell Pointer to the shell structure.
 * @param cmd Pointer to the command structure.
 * @return int 0 if all valid variables were processed (even if not found),
 * 1 if any variable name was invalid.
 */
int ft_unset(t_shell *shell, t_cmd *cmd)
{
	int i;
	int overall_status;

	i = 1; // Start from cmd->args[1]
	overall_status = 0; // Assume success

	// If no arguments are given, unset does nothing and succeeds
	if (cmd->args[1] == NULL)
	{
		return (0);
	}

	// Process each argument
	while (cmd->args[i] != NULL)
	{
		// remove_env_variable will print error for invalid keys
		// and update overall_status if an error occurs.
		if (remove_env_variable(&(shell->env), cmd->args[i]) != 0)
		{
			overall_status = 1; // Mark that at least one error occurred
		}
		i++;
	}
	return (overall_status);
}


int ft_cd(t_shell *shell, t_cmd *cmd)
{
	char *oldpwd_val = NULL;
	char *newpwd_val = NULL;
	const char *target_path = NULL;
	int chdir_result;

	// 1. Получить текущую директорию ДО попытки ее сменить.
	// Это значение станет новым OLDPWD в случае успеха.
	oldpwd_val = getcwd(NULL, 0);
	// getcwd(NULL, 0) выделяет память, которую нужно будет освободить.
	// Если getcwd не смог получить текущий путь (очень редкая ошибка),
	// oldpwd_val будет NULL. Мы должны это учесть.

	// 2. Определить целевую директорию
	if (!cmd->args[1]) // Если `cd` вызван без аргументов
	{
		target_path = get_env_value(shell->env, "HOME");
		if (!target_path)
		{
			fprintf(stderr, "minishell: cd: HOME not set\n");
			free(oldpwd_val); // Освобождаем память, выделенную getcwd
			return (1);
		}
		if (*target_path == '\0') // Если HOME установлен, но пуст
		{
			fprintf(stderr, "minishell: cd: HOME is empty\n");
			free(oldpwd_val);
			return (1);
		}
	}
	else if (ft_strcmp(cmd->args[1], "-") == 0) // Если аргумент "-"
	{
		target_path = get_env_value(shell->env, "OLDPWD");
		if (!target_path)
		{
			fprintf(stderr, "minishell: cd: OLDPWD not set\n");
			free(oldpwd_val);
			return (1);
		}
		if (*target_path == '\0')
		{
				fprintf(stderr, "minishell: cd: OLDPWD is empty\n");
			free(oldpwd_val);
			return (1);
		}
		// Стандартные оболочки печатают директорию, в которую переходят по `cd -`
		printf("%s\n", target_path);
	}
	else // Если аргумент - это путь
	{
		target_path = cmd->args[1];
	}

	// 3. Попытаться сменить директорию
	// Проверяем, что target_path не NULL (например, если HOME/OLDPWD были установлены, но пусты)
	if (!target_path) { // Эта проверка может быть избыточной, если предыдущие уже отработали
		fprintf(stderr, "minishell: cd: no target directory specified\n");
		free(oldpwd_val);
		return (1);
	}

	chdir_result = chdir(target_path);
	if (chdir_result != 0)
	{
		// Использовать strerror(errno) для получения системного сообщения об ошибке
		perror("minishell: cd:  failed");
		free(oldpwd_val);
		return (1);
	}

	// 4. Если chdir успешен, получить новый текущий путь для PWD
	newpwd_val = getcwd(NULL, 0);
	if (!newpwd_val)
	{
		// Это очень маловероятно, если chdir прошел успешно, но лучше проверить
		perror("minishell: cd: getcwd after chdir failed");
		// В этой ситуации OLDPWD может быть обновлен, а PWD нет.
		// Для упрощения, просто не обновляем PWD и возвращаем ошибку.
		// Важно обновить OLDPWD, если oldpwd_val не NULL.
		if (oldpwd_val)
			change_env_value(&(shell->env), "OLDPWD", oldpwd_val);
		free(oldpwd_val);
		return (1);
	}

	// 5. Обновить переменные окружения OLDPWD и PWD
	// Функция change_env_value ДОЛЖНА сама позаботиться о дублировании строк
	// oldpwd_val и newpwd_val, так как они будут освобождены в этой функции.
	if (oldpwd_val) // Устанавливаем OLDPWD только если он был успешно получен
	{
		change_env_value(&(shell->env), "OLDPWD", oldpwd_val);
	}
	change_env_value(&(shell->env), "PWD", newpwd_val);

	// 6. Освободить память, выделенную getcwd
	free(oldpwd_val);
	free(newpwd_val);

	return (0); // Успех
}
