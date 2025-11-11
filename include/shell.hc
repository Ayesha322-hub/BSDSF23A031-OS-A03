#ifndef SHELL_H
#define SHELL_H

#define CMD_LENGTH 1024

#include "history.h"  // this brings in history functions

void shell_loop();
void execute(char **args);
void handle_builtin(char **args);

#endif
