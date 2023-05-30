#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int execute_command(int argc, char *argv[]) {
    // handle built-in shell commands
    if (strcmp(argv[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else {
            chdir(args[1]);
        }
    }
    else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    else {
        // execute external program
        pid_t pid = fork();
        if (pid == 0) {
            execve(args[0], args);
            perror("execve");
            exit(1);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        } else {
            wait(NULL);
        }
    }
}
