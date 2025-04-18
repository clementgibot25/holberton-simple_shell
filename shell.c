#include "shell.h"

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
 * @argc: Number of arguments
 * @argv: Array of arguments
 *
 * Return: Always 0 (Success)
 */
int main(int argc, char *argv[])
{

	char *progname;
	int cmd_count = 0;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *args[MAX_ARGS];
	int arg_count;
	int builtin_result;
	int is_interactive = isatty(STDIN_FILENO);
	(void)argc;

	progname =  strrchr(argv[0], '/');
		if (progname)
			progname++;
		else
			progname = argv[0];

	while (1)
	{
		if (is_interactive)
		{
			write(STDOUT_FILENO, "$ ", 2);
			fflush(stdout);
		}
		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			if (is_interactive)
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
		execute_command(args, progname, cmd_count); /* Execute external command */
	}
	free(line);
	return (0);
}
