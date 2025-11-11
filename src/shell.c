#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
#include "io_redirection.h"
#include "pipes.h"
#include "multitasking.h"  // <-- This fixes print_jobs() error


// Simple tokenizer
char **tokenize(char *cmd, int *argc) {
    char **args = malloc(64 * sizeof(char*));
    int count = 0;
    char *token = strtok(cmd, " \t\r\n");
    while (token != NULL) {
        args[count++] = strdup(token);
        token = strtok(NULL, " \t\r\n");
    }
    args[count] = NULL;
    *argc = count;
    return args;
}

// Check if command contains a pipe
int contains_pipe(char *cmd) {
    return strchr(cmd, '|') != NULL;
}

// Check if command contains redirection
int contains_redirection(char *cmd) {
    return strchr(cmd, '<') != NULL || strchr(cmd, '>') != NULL;
}

void execute_command(char *cmd) {
    if (strchr(cmd, ';') || strchr(cmd, '&')) {
        execute_multitasking(cmd);
        return;
    }

    if (contains_pipe(cmd)) {
        execute_pipe(cmd);
    } else if (contains_redirection(cmd)) {
        execute_redirection(cmd);
    } else {
        int argc;
        char **args = tokenize(cmd, &argc);
        if (argc == 0) { free(args); return; }

        if (strcmp(args[0], "exit") == 0) {
            for (int i = 0; i < argc; i++) free(args[i]);
            free(args);
            exit(0);
        }
        if (strcmp(args[0], "jobs") == 0) {
            print_jobs();
            for (int i = 0; i < argc; i++) free(args[i]);
            free(args);
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("myshell");
            exit(1);
        } else if (pid < 0) {
            perror("fork");
        } else {
            waitpid(pid, NULL, 0);
        }

        for (int i = 0; i < argc; i++) free(args[i]);
        free(args);
    }
}
