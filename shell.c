#include "shell.h"

/**
 * execute_command - Executes a command with its arguments
 * @argv: Array of command and arguments
 * @progname: Name of the program
 * @cmd_count: Number of commands executed
 * Return: 1 if command was executed, 0 if not
 */
int execute_command(char *argv[], char *progname, int cmd_count)
{
	pid_t pid;
	char *command_path;

	/* Find command in PATH */
	command_path = find_command(argv[0]);
	if (!command_path)
	{
		fprintf(stderr, "%s: %d: %s: not found\n", progname, cmd_count, argv[0]);
		return (0);
	}

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		free(command_path);
		return (0);
	}
	else if (pid == 0)
	{
		/* Child process */
		if (execve(command_path, argv, environ) == -1)
		{
			perror(argv[0]);
			free(command_path);
			exit(1);
		}
	}
	else
	{
		/* Parent proceses */
		wait(NULL);
		free(command_path);
	}
	return (1);
}

/**
 * main - Entry point of the simple shell program
 *
 * Description: This function implements a simple shell that:
 * - Displays a prompt ($)
 * - Reads user input
 * - Parses command and arguments
 * - Handles PATH
 * - Implements exit and env built-ins
 * - Executes commands using execve
 * - Handles errors and EOF condition
 * - Continues until the user types exit or EOF (Ctrl+D)
 *
 * Return: Always 0 (Success)
 */
int main(int argc, char *argv[])
{

	int cmd_count = 0;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *args[MAX_ARGS];
	int arg_count;
	int builtin_result;
	(void)argc;

	while (1)
	{
		if (isatty(STDIN_FILENO))
		{
			printf("$ ");
			fflush(stdout);
		}
		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			printf("\n");
			break;
		}
		if (line[nread - 1] == '\n') /* Remove newline character */
			line[nread - 1] = '\0';
		if (strlen(line) == 0) /* Skip empty lines */
			continue;
		arg_count = parse_input(line, args); /* Parse command and arguments */
		if (arg_count == 0) /* Skip if no command was entered */
			continue;
		builtin_result = handle_builtins(args); /* Handle built-in commands */
		if (builtin_result == 2) /* exit command */
		{
			free(line);
			return (0);
		}
		if (builtin_result == 1) /* other built-in handled */
			continue;
		cmd_count++;
		execute_command(args, argv[0], cmd_count); /* Execute external command */
	}
	free(line);
	return (0);
}
