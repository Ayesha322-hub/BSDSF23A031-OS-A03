#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "shell.h"

int handle_builtin(char **args) {
    if (args[0] == NULL) {
        return 1; // Empty command
    }

    // exit command
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // cd command
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd");
        }
        return 1;
    }

    // help command
    else if (strcmp(args[0], "help") == 0) {
        printf("Ayesha's Shell — Built-in Commands:\n");
        printf("  cd <directory>  : Change current directory\n");
        printf("  help            : Display this help message\n");
        printf("  jobs            : Show job control info (not yet implemented)\n");
        printf("  exit            : Exit the shell\n");
        return 1;
    }

    // jobs command
    else if (strcmp(args[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    return 0; // Not a built-in command
}
