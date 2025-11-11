#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

// Optional: custom completion function (you can expand later)
char *command_generator(const char *text, int state) {
    static int list_index, len;
    static const char *commands[] = {"ls", "cat", "mkdir", "rm", "echo", "pwd", "cd", "exit"};
    const int n_commands = sizeof(commands) / sizeof(char*);
    
    if (!state) { // first call
        list_index = 0;
        len = strlen(text);
    }

    while (list_index < n_commands) {
        const char *name = commands[list_index];
        list_index++;
        if (strncmp(name, text, len) == 0)
            return strdup(name);
    }
    return NULL;
}

// Register completion function with readline
char **custom_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;  // disable default filename completion
    return rl_completion_matches(text, command_generator);
}

int main() {
    char *cmd;

    // Bind Tab key to our custom completion function
    rl_attempted_completion_function = custom_completion;

    // Load previous history
    read_history(".myshell_history");

    while (1) {
        // readline provides prompt, editing, and history
        cmd = readline("myshell> ");

        if (!cmd) {  // Ctrl+D pressed
            printf("\nExiting shell...\n");
            break;
        }

        // Skip empty input
        if (*cmd) {
            add_history(cmd);  // add to history
            char **args = split_command(cmd);
            execute(args);
           
        }

        free(cmd);  // free memory allocated by readline
    }

    // Save history on exit
    write_history(".myshell_history");

    return 0;
}
