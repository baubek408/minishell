/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 15:35:09 by bmynbyae          #+#    #+#             */
/*   Updated: 2025/04/22 14:44:28 by bmynbyae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	cmd_count(char *line)
{
	int	i;
	int	counter;
	int	flag;

	i = 0;
	counter = 1;
	flag = 0;
	while (line[i])
	{
		if (line[i] == '"' && flag == 0)
			flag = 1;
		else if (line[i] == '"' && flag == 1)
			flag = 0;
		if (line[i] == '|' && flag == 0)
			counter++;
		if (line[i] == ';' && flag == 0)
			counter++;
		i++;
	}
	return (counter);
}

t_cmd	**cmd_init(char *line)
{
	int		cmd_len;
	t_cmd	**new;
	int		i;

	if (!line)
		return (NULL);
	cmd_len = cmd_count(line);
	new = malloc(sizeof(t_cmd *) * cmd_len);
	if (!new)
		return (NULL);
	i = 0;
	while (i < cmd_len)
	{
		new[i]->cmd = NULL;
		new[i]->av = NULL;
		i++;
	}
	return (new);
}


// t_env **cmd_init(char *line)
// {
// 	if(!line)
// 		return(NULL);
// 	int cmd_len = cmd_count(line);
// 	t_cmd **new;

// 	new = malloc(sizeof(t_cmd *) * cmd_len);
// 	if(!new)
// 		return(NULL);
// 	int i = 0;
// 	while (i < cmd_len)
// 	{
// 		new[i]->cmd = NULL;
// 		new[i]->av = NULL;
// 		i++;
// 	}
// 	return(new);
// }

// void cmd_divider(char *line)
// {
// 	t_env *cmd = NULL;
//     int i = 0;

// 	while (line[i])
//     {
//         char *c = ft_strchr(line[i],' ');
//         if(c)
//         {
//             int key_len = c - line[i];
//             char *key = ft_strndup(line[i],key_len);
//             char *val = ft_strdup(c+1);
//             add_env_node(head,key,val);
//         }
//         i++;
//     }
// }

void	print_env_list(t_env *env)
{
	while (env)
	{
		if (env->val)
			printf("%s=%s\n", env->key, env->val);
		env = env->next;
	}
}

int main(int ac, char **av, char **envp)
{
	char *line;
	// t_cmd *cmd_head;
	t_env *head = NULL;

	(void)ac;
	(void)av;
	head = init_env(envp);
	print_env_list(head);
	char *m = NULL;
	m = get_env_value(head,"OLDPWD");
	if(m)
		printf("%s\n",m);

	while (1)
	{
		line = readline("minishell$ ");
		if(!line)
		break;

		if(*line)
		add_history(line);
		// cmd_head = cmd_init(line);
		printf("%d\n", cmd_checker(line));
		// printf("minishell$ %s\n",line);
		free(line);
	}
	return(0);
}
