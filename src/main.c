#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

int main() {
    char *cmd;

    // Load previous history
    read_history(".myshell_history");

    while (1) {
        cmd = readline("myshell> ");
        if (!cmd) break;

        if (*cmd) {
            add_history(cmd);
            execute_command(cmd);  // unified execution
        }

        free(cmd);
    }

    write_history(".myshell_history");
    return 0;
}
