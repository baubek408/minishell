#include "minishell.h"

int buildin_checker(char *cmd)
{
    char *builtins[] = {"cd", "echo", "pwd", "exit", "export", "unset", "env"};
    int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
    int i = 0;

    if (!cmd)
        return (0);
    while (i < num_builtins)
    {
        if (ft_strcmp(cmd, builtins[i]) == 0)
            return (1);
        i++;
    }
    return (0);
}
int find_buildin(t_shell *shell, t_cmd *cmd)
{
	if (!cmd || !cmd->cmd)
	{
		perror("minishell: invalid command in find_buildin()\n");
		shell->exit_status = 1;
		return (shell->exit_status);
	}
	if (!ft_strcmp(cmd->cmd, "echo"))
		return(shell->exit_status = ft_echo(shell,cmd));
	else if (!ft_strcmp(cmd->cmd, "env"))
		return(shell->exit_status = ft_env(shell));
	else if (!ft_strcmp(cmd->cmd, "pwd"))
		return(shell->exit_status = ft_pwd());
	else if (!ft_strcmp(cmd->cmd, "export"))
		return(shell->exit_status = ft_export(shell,cmd));
	else if (!ft_strcmp(cmd->cmd, "exit"))
		return(ft_exit(shell));
	else if (!ft_strcmp(cmd->cmd, "unset"))
		return(shell->exit_status = ft_unset(shell, cmd));
	else if (!ft_strcmp(cmd->cmd, "cd"))
		return(shell->exit_status = ft_cd(shell, cmd));
}
