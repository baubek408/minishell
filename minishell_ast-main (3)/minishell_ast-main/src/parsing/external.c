#include "minishell.h"


void free_split(char **arr)
{
    if (!arr)
        return;
    for (int i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}



char *ft_strjoin_free(char *s1, const char *s2)
{
    char *joined = ft_strjoin(s1, s2);
    free(s1);
    return joined;
}

char **env_to_envp_array(t_env *env)
{
    int count = 0;
    t_env *tmp = env;

    while (tmp)
    {
        count++;
        tmp = tmp->next;
    }

    char **envp = malloc(sizeof(char *) * (count + 1));
    if (!envp)
        return NULL;

    tmp = env;
    for (int i = 0; i < count; i++)
    {
        size_t key_len = strlen(tmp->key);
        size_t val_len = strlen(tmp->val);
        char *entry = malloc(key_len + val_len + 2); // '=' + '\0'

        if (!entry)
        {
            // Free everything on error
            for (int j = 0; j < i; j++)
                free(envp[j]);
            free(envp);
            return NULL;
        }

        sprintf(entry, "%s=%s", tmp->key, tmp->val);
        envp[i] = entry;
        tmp = tmp->next;
    }

    envp[count] = NULL;
    return envp;
}

char *find_command_path(char *cmd, t_env *envp)
{
	if (ft_strchr(cmd, '/')) // absolute or relative path
		return ft_strdup(cmd);

	char *path_env = get_env_value(envp,"PATH");
	if (!path_env)
		return NULL;

	char **paths = ft_split(path_env, ':');
	char *full_path = NULL;

	for (int i = 0; paths[i]; i++)
	{
		char *tmp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);

		if (access(full_path, X_OK) == 0)
		{
			free_split(paths);
			return full_path;
		}
		free(full_path);
	}
	free_split(paths);
	return NULL;
}
int is_directory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == 0)
        return S_ISDIR(statbuf.st_mode);
    return 0;
}

int exec_external(t_shell *shell, t_cmd *cmd)
{
    pid_t pid;
    int status;
    char **envp = env_to_envp_array(shell->env);
    char *full_path = find_command_path(cmd->cmd, shell->env);

    if (!full_path)
    {
        fprintf(stderr, "%s: command not found\n", cmd->cmd);
        free_split(envp);
        shell->exit_status = 127;
        return 127;
    }

	if (is_directory(full_path))
	{
		fprintf(stderr, "%s: Is a directory\n", full_path);
		exit(126); // Bash uses 126 for "command found but not executable"
	}
    pid = fork();
    if (pid == -1)
    {
        perror("minishell: fork");
        shell->exit_status = 1;
        return 1;
    }

    if (pid == 0)
    {
        // Child process
        execve(full_path, cmd->args, envp);
        perror("minishell: execve failed");
        exit(1);
    }
    else
    {
        // Parent process waits for child
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            shell->exit_status = WEXITSTATUS(status);
        else
            shell->exit_status = 1;
    }

    free_split(envp);
    free(full_path);
    return shell->exit_status;
}
