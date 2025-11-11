#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "io_redirection.h"
#include "shell.h"

void execute_redirection(char *cmd) {
    char *cmd_copy = strdup(cmd);
    char *token;
    char *saveptr;
    char *argv[64];
    int argc = 0;

    char *input_file = NULL;
    char *output_file = NULL;

    token = strtok_r(cmd_copy, " ", &saveptr);
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
        if (input_file) {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0) { perror("open input"); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (output_file) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("open output"); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    } else if (pid < 0) {
        perror("fork");
    } else {
        waitpid(pid, NULL, 0);
    }

    free(cmd_copy);
}
