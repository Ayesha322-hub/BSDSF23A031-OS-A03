#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // ✅ for fork(), execl()
#include <sys/wait.h>    // ✅ for waitpid(), WEXITSTATUS()
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

// Helper function to detect keyword
int starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// ✅ Moved this here so it's compiled with main.c
int run_command_and_get_status(const char *cmd) {
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        perror("exec failed");
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    } else {
        perror("fork failed");
        return 1;
    }
}

int main() {
    char *cmd;
    char *if_cmd = NULL;           
    char *then_block[100];         
    char *else_block[100];         
    int then_count = 0, else_count = 0;
    int in_if = 0, in_then = 0, in_else = 0;

    read_history(".myshell_history");

    while (1) {
        cmd = readline("myshell> ");
        if (!cmd) break;
        if (!*cmd) { free(cmd); continue; }

        // --- Start of IF Block Parsing ---
        if (starts_with(cmd, "if ")) {
            in_if = 1;
            if_cmd = strdup(cmd + 3);  
        } 
        else if (in_if && starts_with(cmd, "then")) {
            in_then = 1;
        } 
        else if (in_if && starts_with(cmd, "else")) {
            in_then = 0;
            in_else = 1;
        } 
        else if (in_if && starts_with(cmd, "fi")) {
            // Execute IF command first
            int status = run_command_and_get_status(if_cmd);

            if (status == 0) {
                for (int i = 0; i < then_count; i++) {
                    execute_command(then_block[i]);
                    free(then_block[i]);
                }
            } else {
                for (int i = 0; i < else_count; i++) {
                    execute_command(else_block[i]);
                    free(else_block[i]);
                }
            }

            in_if = in_then = in_else = 0;
            then_count = else_count = 0;
            free(if_cmd);
            if_cmd = NULL;
        } 
        else if (in_then) {
            then_block[then_count++] = strdup(cmd);
        } 
        else if (in_else) {
            else_block[else_count++] = strdup(cmd);
        } 
        else {
            add_history(cmd);
            execute_command(cmd);
        }

        free(cmd);
    }

    write_history(".myshell_history");
    return 0;
}
