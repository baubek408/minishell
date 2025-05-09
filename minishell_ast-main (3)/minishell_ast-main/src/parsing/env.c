#include "minishell.h"








char *get_env_value(t_env *env, const char *key)
{
    while (env)
    {
        if (strcmp(env->key, key) == 0)
            return env->val;
        env = env->next;
    }
    return NULL;
}

t_env *find_env_key(t_env *env, const char *key)
{
	while (env)
	{
		if (strcmp(env->key, key) == 0)
			return env;
		env = env->next;
	}
	return NULL;
}

void change_env_value(t_env **env_list_ptr, const char *key, const char *value)
{
    t_env *current;
    t_env *prev;
    char *new_val_dup;
    char *new_key_dup;

    if (!env_list_ptr || !key) // value может быть NULL (для пустой переменной)
        return; // Возвращаем ошибку, если нет списка или ключа

    current = *env_list_ptr;
    prev = NULL;

    // 1. Поиск существующей переменной
    while (current != NULL)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            new_val_dup = ft_strdup(value); // Дублируем новое значение
            if (!new_val_dup && value != NULL) // Ошибка, если value не NULL, а strdup вернул NULL
            {
                perror("minishell: change_env_value: ft_strdup for value failed");
                // Не меняем значение, если не удалось выделить память
                return;
            }
            free(current->val);    // Освобождаем старое значение
            current->val = new_val_dup;  // Присваиваем новое дублированное значение
            return; // Успешно обновлено
        }
        prev = current;
        current = current->next;
    }

    // 2. Если переменная не найдена, добавляем новую
    new_key_dup = ft_strdup(key);
    if (!new_key_dup)
    {
        perror("minishell: change_env_value: ft_strdup for key failed");
        return;
    }
    new_val_dup = ft_strdup(value);
    if (!new_val_dup && value != NULL)
    {
        perror("minishell: change_env_value: ft_strdup for value failed");
        free(new_key_dup);
        return;
    }

    current = (t_env *)malloc(sizeof(t_env));
    if (!current)
    {
        perror("minishell: change_env_value: malloc for new node failed");
        free(new_key_dup);
        free(new_val_dup); // new_val_dup может быть NULL, если value был NULL
        return;
    }
    current->key = new_key_dup;
    current->val = new_val_dup;
    current->next = NULL;

    if (prev == NULL) // Список был пуст или добавляем в начало (если *env_list_ptr был NULL)
    {
        *env_list_ptr = current;
    }
    else // Добавляем в конец списка
    {
        prev->next = current;
    }
    return; // Успешно добавлено
}

void	ft_free_env(t_env *env)
{
	t_env *tmp;

	while (env)
	{
		tmp = env->next;

		if (env->key)
			free(env->key);
		if (env->val)
			free(env->val);
		free(env);

		env = tmp;
	}
}
