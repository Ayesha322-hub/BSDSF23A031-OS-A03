#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "io_redirection.h"

void execute_redirection(char *cmd) {
    char *input_file = NULL;
    char *output_file = NULL;
    char *args_str = strdup(cmd);

    // Parse for < and >
    char *token;
    char *saveptr;
    char *argv[64];
    int argc = 0;

    token = strtok_r(args_str, " ", &saveptr);
    while (token) {
        if (strcmp(token, "<") == 0) {
            token = strtok_r(NULL, " ", &saveptr);
            input_file = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok_r(NULL, " ", &saveptr);
            output_file = token;
        } else {
            argv[argc++] = token;
        }
        token = strtok_r(NULL, " ", &saveptr);
    }
    argv[argc] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in < 0) { perror("open input"); exit(EXIT_FAILURE); }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (output_file) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) { perror("open output"); exit(EXIT_FAILURE); }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        execvp(argv[0], argv);
        perror("myshell");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

    free(args_str);
}
