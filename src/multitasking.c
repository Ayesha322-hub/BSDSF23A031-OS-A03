#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "multitasking.h"
#include "shell.h"

static job_t *job_list = NULL;

// Add a background job
void add_job(pid_t pid, const char *cmd) {
    job_t *new_job = malloc(sizeof(job_t));
    new_job->pid = pid;
    strncpy(new_job->command, cmd, 255);
    new_job->command[255] = '\0';
    new_job->next = job_list;
    job_list = new_job;
}

// Remove a completed job
void remove_job(pid_t pid) {
    job_t **indirect = &job_list;
    while (*indirect) {
        if ((*indirect)->pid == pid) {
            job_t *tmp = *indirect;
            *indirect = tmp->next;
            free(tmp);
            return;
        }
        indirect = &((*indirect)->next);
    }
}

// Print all active background jobs
void print_jobs() {
    job_t *curr = job_list;
    printf("Active background jobs:\n");
    while (curr) {
        printf("[%d] %s\n", curr->pid, curr->command);
        curr = curr->next;
    }
}

// Execute commands with ; chaining and & background
void execute_multitasking(char *cmd) {
    char *cmd_copy = strdup(cmd);
    char *segment = strtok(cmd_copy, ";");

    while (segment) {
        // Trim leading/trailing spaces
        while (*segment == ' ') segment++;
        char *end = segment + strlen(segment) - 1;
        while (end > segment && (*end == ' ' || *end == '\n')) { *end = '\0'; end--; }

        bool background = false;
        if (strlen(segment) > 0 && segment[strlen(segment)-1] == '&') {
            background = true;
            segment[strlen(segment)-1] = '\0'; // remove &
        }

        pid_t pid = fork();
        if (pid == 0) { // child
            execute_command(segment);
            exit(0);
        } else if (pid < 0) {
            perror("fork");
        } else { // parent
            if (background) {
                add_job(pid, segment);
            } else {
                waitpid(pid, NULL, 0);
            }
        }

        segment = strtok(NULL, ";");
    }

    free(cmd_copy);

    // Reap any finished background processes
    int status;
    pid_t wpid;
    while ((wpid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_job(wpid);
    }
}
