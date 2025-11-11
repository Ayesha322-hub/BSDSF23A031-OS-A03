#ifndef SHELL_H
#define SHELL_H

char **split_command(char *cmd);
void free_args(char **args);
void execute(char **args);

#endif
