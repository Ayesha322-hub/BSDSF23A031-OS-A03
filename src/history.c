#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

// History storage
static char* history[HISTORY_SIZE];
static int count = 0;  // total commands entered
static int next = 0;   // next index to insert

// Add a command to history
void add_history(const char *cmd) {
    if(history[next] != NULL) {
        free(history[next]); // free old command if overwritten
    }
    history[next] = strdup(cmd); // store a copy
    next = (next + 1) % HISTORY_SIZE;
    if(count < HISTORY_SIZE) count++;
}

// Print all commands
void print_history() {
    for(int i = 0; i < count; i++) {
        int idx = (next + i) % HISTORY_SIZE;
        printf("%d %s\n", i+1, history[idx]);
    }
}

// Get the nth command (1-based index)
char* get_history_command(int index) {
    if(index < 1 || index > count) return NULL;
    int idx = (next + index - 1) % HISTORY_SIZE;
    return history[idx];
}
