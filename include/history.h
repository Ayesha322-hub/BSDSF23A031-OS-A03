#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 20   // store last 20 commands
#define CMD_LENGTH 1024   // max command length

// Functions to manage command history
void add_history(const char *cmd);
void print_history();
char* get_history_command(int index);

#endif
