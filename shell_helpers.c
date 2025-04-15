#include "sh_header.h"

/**
 * _getenv - Gets the value of an environment variable
 * @name: Name of the environment variable
 * Return: Value of the environment variable or NULL if not found
 */
char *_getenv(const char *name)
{
	int i = 0;
	size_t len = strlen(name);

	while (environ[i])
	{
		if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
			return (&environ[i][len + 1]);
		i++;
	}
	return (NULL);
}

/**
 * find_command - Finds the full path of a command
 * @command: Command to find
 * Return: Full path of the command or NULL if not found
 */
char *find_command(char *command)
{
	char *path, *path_copy, *dir, *full_path;

	if (strchr(command, '/')) /* If cmd have '/', chck if it exists and is exe */
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}

	path = _getenv("PATH"); /* Get PATH environment variable */
	if (!path)
		return (NULL);

	path_copy = strdup(path);
	if (!path_copy)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir)
	{
		full_path = malloc(strlen(dir) + strlen(command) + 2);
		if (!full_path)
		{
			free(path_copy);
			return (NULL);
		}

		sprintf(full_path, "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}

		free(full_path);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * print_env - Prints the current environment
 */
void print_env(void)
{
	int i = 0;

	while (environ[i])
	{
		printf("%s\n", environ[i]);
		i++;
	}
}

/**
 * parse_input - Parses the input line into command and arguments
 * @line: Input line to parse
 * @argv: Array to store parsed arguments
 * Return: Number of arguments parsed
 */
int parse_input(char *line, char *argv[])
{
	int argc = 0;

	argv[argc] = strtok(line, " \t");
	while (argv[argc] != NULL && argc < MAX_ARGS - 1)
	{
		argc++;
		argv[argc] = strtok(NULL, " \t");
	}
	argv[argc] = NULL;
	return (argc);
}

/**
 * handle_builtins - Handles built-in commands
 * @argv: Array of command and arguments
 * Return: 1 if built-in was handled, 0 if not
 */
int handle_builtins(char *argv[])
{
	if (strcmp(argv[0], "exit") == 0)
		return (2); /* Special return value for exit */
	if (strcmp(argv[0], "env") == 0)
	{
		print_env();
		return (1);
	}
	return (0);
}
