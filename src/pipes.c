#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "pipes.h"
#include "shell.h"

// Execute a single pipe command
void execute_pipe(char *cmd) {
    char *cmd_copy = strdup(cmd);
    char *cmd1_str = strtok(cmd_copy, "|");
    char *cmd2_str = strtok(NULL, "|");

    if (!cmd2_str) { 
        execute_command(cmd1_str); 
        free(cmd_copy); 
        return; 
    }

    int fd[2];
    pipe(fd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]); close(fd[1]);
        execute_command(cmd1_str);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]); close(fd[1]);
        execute_command(cmd2_str);
        exit(0);
    }

    close(fd[0]); close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    free(cmd_copy);
}
