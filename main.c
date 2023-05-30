#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#define MAXLINE 1024
#define MAXARGS 128
extern char **environ; // Declare global variable to hold environment variables
/* Function declarations */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
/* Main function */
int main(int argc, char **argv)
{
    char cmdline[MAXLINE];
    while (1) // Infinite loop that reads command lines from the user
    {
        printf("$ "); // Print prompt
        fflush(stdout); // Flush output buffer
        if (fgets(cmdline, MAXLINE, stdin) == NULL) // Read input from user
        {
            exit(0); // Exit program if there is no input
        }
        eval(cmdline); // Execute command line
    }
}
/* Function to execute command line */
void eval(char *cmdline)
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    strcpy(buf, cmdline); // Copy command line to buffer
    bg = parseline(buf, argv); // Parse command line and determine whether it should be run in background
    if (argv[0] == NULL) { // If there are no arguments, return
        return;
    }
    if (!builtin_command(argv)) { // If the command is not a built-in command
        if ((pid = fork()) == 0) { // Fork a child process
            if (argv[0][0] == '/') { // Check if the command contains an absolute path
                execve(argv[0], argv, environ); // Execute the command at the absolute path
            } else {
                char *path = getenv("PATH"); // Get the PATH environment variable
                char *dir = strtok(path, ":"); // Split the PATH into directories using ":" as a delimiter
                while (dir != NULL) {
                    char command[MAXLINE];
                    sprintf(command, "%s/%s", dir, argv[0]); // Construct the full path to the command
                    execve(command, argv, environ); // Try to execute the command
                    dir = strtok(NULL, ":"); // Move on to the next directory in PATH
                }
                fprintf(stderr, "Command not found: %s\n", argv[0]); // Error handling
                exit(1);
            }
        }
        if (!bg) { // If the command is not running in the background, wait for child process to finish executing
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                fprintf(stderr, "waitfg: waitpid error\n");
            }
        } else { // If the command is running in the background, print its PID
            printf("%d %s", pid, cmdline);
        }
    }
    return; // Return control to main function
}
/* Function to determine whether command is a built-in command and execute it */
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "exit")) { // If the command is "exit", exit the program
        exit(0);
    } else if (!strcmp(argv[0], "env")) { // If the command is "env", print the environment variables
        char **env = environ;
        while (*env != NULL) {
            printf("%s\n", *env);
            env++;
        }
        return 1;
    } else if (!strcmp(argv[0], "&")) { // If the command is "&", return true to indicate that it should be run in the background
        return 1;
    }
    return 0; // Otherwise, return false
}
/* Function to parse command line and separate it into an array of arguments */
int parseline(char *buf, char **argv)
{
    char *delim;
    int argc;
    int bg;
    buf[strlen(buf)-1] = ' '; // Replace newline character with space character
    while (*buf && (*buf == ' ')) // Skip leading whitespace
    {
        buf++;
    }
    argc = 0;
    while ((delim = strchr(buf, ' '))) // Separate arguments using whitespace as delimiter
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) // Skip whitespace between arguments
        {
            buf++;
        }
    }
    argv[argc] = NULL; // Set last argument to NULL
    if (argc == 0) // If there are no arguments, return true to indicate that the command should be run in the background
    {
        return 1;
    }
    if ((bg = (*argv[argc-1] == '&'))) // If the last argument is "&", remove it and return true to indicate that the command should be run in the background
        argv[--argc] = NULL;
    {
    }
    return bg; // Otherwise, return false
}