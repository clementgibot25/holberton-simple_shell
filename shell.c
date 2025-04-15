#include "sh_header.h"

/**
 * execute_command - Executes a command with its arguments
 * @argv: Array of command and arguments
 * Return: 1 if command was executed, 0 if not
 */
int execute_command(char *argv[])
{
    pid_t pid;
    char *command_path;

    /* Find command in PATH */
    command_path = find_command(argv[0]);
    if (!command_path)
    {
        fprintf(stderr, "%s: command not found\n", argv[0]);
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
        argv[0] = command_path;
        if (execve(command_path, argv, environ) == -1)
        {
            perror(argv[0]);
            free(command_path);
            exit(1);
        }
    }
    else
    {
        /* Parent process */
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
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *argv[MAX_ARGS];
    int argc;
    int builtin_result;

    while (1)
    {
        printf("$ ");
        fflush(stdout);

        nread = getline(&line, &len, stdin);
        if (nread == -1)
        {
            printf("\n");
            break;
        }

        /* Remove newline character */
        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        /* Skip empty lines */
        if (strlen(line) == 0)
            continue;

        /* Parse command and arguments */
        argc = parse_input(line, argv);

        /* Skip if no command was entered */
        if (argc == 0)
            continue;

        /* Handle built-in commands */
        builtin_result = handle_builtins(argv);
        if (builtin_result == 2) /* exit command */
        {
            free(line);
            return (0);
        }
        if (builtin_result == 1) /* other built-in handled */
            continue;

        /* Execute external command */
        execute_command(argv);
    }

    free(line);
    return (0);
}