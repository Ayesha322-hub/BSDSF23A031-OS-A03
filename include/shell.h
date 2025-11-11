#ifndef SHELL_H
#define SHELL_H

int run_command_and_get_status(const char *cmd);

void execute_command(char *cmd);

// Multitasking support
void execute_multitasking(char *cmd);

#endif // SHELL_H
