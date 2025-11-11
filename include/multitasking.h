#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdbool.h>

// Structure for background jobs
typedef struct job {
    pid_t pid;
    char command[256];
    struct job *next;
} job_t;

// Add a background job
void add_job(pid_t pid, const char *cmd);

// Remove a completed job
void remove_job(pid_t pid);

// Print all active background jobs
void print_jobs();

// Execute command with chaining (;) and background (&)
void execute_multitasking(char *cmd);

#endif // MULTITASKING_H
