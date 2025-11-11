#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "if_then.h"
#include "shell.h"

#define MAX_BLOCK_LINES 256

// trim leading & trailing whitespace (modifies the string)
static char *trim(char *s) {
    if (!s) return s;
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        *end = '\0';
        end--;
    }
    return s;
}

// Tokenize a command string into argv (returns malloc'd argv array; caller must free)
static char **tokenize_for_exec(char *cmd, int *argc_out) {
    int cap = 64;
    char **argv = malloc(cap * sizeof(char*));
    int argc = 0;

    char *p = cmd;
    char *tok;
    // simple strtok split on whitespace (handles usual cases)
    tok = strtok(p, " \t\r\n");
    while (tok) {
        if (argc + 1 >= cap) {
            cap *= 2;
            argv = realloc(argv, cap * sizeof(char*));
        }
        argv[argc++] = strdup(tok);
        tok = strtok(NULL, " \t\r\n");
    }
    argv[argc] = NULL;
    *argc_out = argc;
    return argv;
}

// Execute a single command string and return its exit status (-1 on fork/exec error)
static int exec_and_wait(char *cmdline) {
    if (!cmdline || strlen(trim(cmdline)) == 0) return 127; // treat empty as failure

    // Need a writable copy for tokenization
    char *copy = strdup(cmdline);
    int argc;
    char **argv = tokenize_for_exec(copy, &argc);
    if (argc == 0) {
        free(copy);
        free(argv);
        return 127;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // child: execute using execvp
        execvp(argv[0], argv);
        perror("execvp");
        // If exec fails:
        _exit(127);
    } else if (pid < 0) {
        perror("fork");
        // cleanup
        for (int i = 0; i < argc; ++i) free(argv[i]);
        free(argv);
        free(copy);
        return -1;
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            for (int i = 0; i < argc; ++i) free(argv[i]);
            free(argv);
            free(copy);
            return -1;
        }
        int exitcode = 1;
        if (WIFEXITED(status)) exitcode = WEXITSTATUS(status);
        // cleanup
        for (int i = 0; i < argc; ++i) free(argv[i]);
        free(argv);
        free(copy);
        return exitcode;
    }
}

// Main handler that reads lines until fi and executes the conditional blocks
void handle_if_block(char *first_line) {
    // We'll collect strings for then_block and else_block
    char *then_lines[MAX_BLOCK_LINES];
    char *else_lines[MAX_BLOCK_LINES];
    int then_count = 0, else_count = 0;

    // determine the condition command
    char *cond_cmd = NULL;
    char *fl_trim = strdup(first_line ? first_line : "");
    char *t = trim(fl_trim);

    if (strncmp(t, "if ", 3) == 0) {
        cond_cmd = strdup(trim(t + 3));
    } else if (strcmp(t, "if") == 0) {
        // read next non-empty line that will be the condition
        char *next;
        while (1) {
            next = readline("> ");
            if (!next) { free(fl_trim); return; } // EOF
            add_history(next);
            char *ntrim = trim(next);
            if (ntrim && strlen(ntrim) > 0) {
                cond_cmd = strdup(ntrim);
                free(next);
                break;
            }
            free(next);
        }
    } else {
        // malformed first_line; treat as normal command fallback
        free(fl_trim);
        if (t && strlen(t) > 0) execute_command(t);
        return;
    }

    free(fl_trim);

    // Read until 'then'
    char *line = NULL;
    while (1) {
        line = readline("> ");
        if (!line) { free(cond_cmd); return; } // EOF
        add_history(line);
        char *lt = trim(line);
        if (lt && strcmp(lt, "then") == 0) {
            free(line);
            break;
        }
        // if user typed something else before 'then' we ignore it (shell syntax expects then)
        free(line);
    }

    // Read then-block until 'else' or 'fi'
    int in_else = 0;
    while (1) {
        line = readline("> ");
        if (!line) { break; }
        add_history(line);
        char *lt = trim(line);

        if (lt && strcmp(lt, "else") == 0) {
            in_else = 1;
            free(line);
            continue;
        }
        if (lt && strcmp(lt, "fi") == 0) {
            free(line);
            break;
        }
        if (!in_else) {
            if (then_count < MAX_BLOCK_LINES) then_lines[then_count++] = strdup(lt);
        } else {
            if (else_count < MAX_BLOCK_LINES) else_lines[else_count++] = strdup(lt);
        }
        free(line);
    }

    // Execute condition command and check exit code
    int cond_status = exec_and_wait(cond_cmd);
    free(cond_cmd);

    // If cond_status == 0 -> run then_lines else run else_lines
    if (cond_status == 0) {
        for (int i = 0; i < then_count; ++i) {
            if (then_lines[i] && strlen(then_lines[i]) > 0) {
                execute_command(then_lines[i]);
            }
            free(then_lines[i]);
        }
        // free any else lines
        for (int i = 0; i < else_count; ++i) free(else_lines[i]);
    } else {
        for (int i = 0; i < else_count; ++i) {
            if (else_lines[i] && strlen(else_lines[i]) > 0) {
                execute_command(else_lines[i]);
            }
            free(else_lines[i]);
        }
        // free then lines too
        for (int i = 0; i < then_count; ++i) free(then_lines[i]);
    }
}
