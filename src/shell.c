#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.hc"     // contains CMD_LENGTH and handle_builtin declaration
#include "history.h"

void handle_builtin(char **args) {
    if(strcmp(args[0], "history") == 0) {
        print_history();
    }
    // Add other built-ins like exit, cd if needed
}

void shell_loop() {
    char line[CMD_LENGTH];

    while(1) {
        printf("myshell> ");
        if(!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0; // remove newline

        if(strlen(line) == 0) continue;

        // Task 4: Handle !n re-execution
        if(line[0] == '!') {
            int n = atoi(line + 1);
            char* cmd = get_history_command(n);
            if(cmd) {
                printf("%s\n", cmd);
                strcpy(line, cmd);
            } else {
                printf("No such command in history.\n");
                continue;
            }
        }

        add_history(line); // store command

        // Tokenize the command
        char *args[100];
        int i = 0;
        char *token = strtok(line, " ");
        while(token) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Handle built-ins
        if(args[0]) {
            if(strcmp(args[0], "history") == 0) {
                handle_builtin(args);
                continue;
            }
        }

        // Execute external commands
        execute(args);
    }
}
