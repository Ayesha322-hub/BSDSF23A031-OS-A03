#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "pipes.h"
#include "shell.h"

void execute_pipe(char *cmd) {
    char *cmd1 = strtok(cmd, "|");
    char *cmd2 = strtok(NULL, "|");

    if (!cmd2) return; // no second command

    int fd[2];
    pipe(fd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Left child: write end
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execute_command(cmd1);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Right child: read end
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execute_command(cmd2);
        exit(0);
    }

    // Parent closes pipe
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

