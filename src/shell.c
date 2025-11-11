#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

#define MAX_ARGS 64
#define DELIM " \t\r\n\a"

// Split command string into arguments
char **split_command(char *cmd) {
    int bufsize = MAX_ARGS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(cmd, DELIM);
    while (token != NULL) {
        tokens[position++] = strdup(token);  // strdup to copy each token
        if (position >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// Free the array of arguments
void free_args(char **args) {
    if (!args)
        return;
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

// Execute command
void execute(char **args) {
    if (args[0] == NULL)  // empty command
        return;

    // Handle built-in command: exit
    if (strcmp(args[0], "exit") == 0) {
        free_args(args);
        exit(0);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Forking error
        perror("myshell");
    } else {
        // Parent waits for child
        int status;
        waitpid(pid, &status, 0);
    }

    free_args(args);  // free allocated argument memory
}
