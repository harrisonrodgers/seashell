/**
 * @file seashell.h
 * @brief Main shell header.
 * @author Harrison Rodgers
 * @version 1.0
 * @date 2015-04-23
 */
#ifndef SEASHELL_H
#define SEASHELL_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

//#define DEBUG
#define MAX_ARGS 64
#define MAX_BUFFER 1024
#define SEPARATORS " \t\n"

/* structure to hold information relevant to the current command being evaluated/executed */
typedef struct Command {
    unsigned short is_stderr_append : 1;
    unsigned short is_stdout_append : 1;
    unsigned short is_background : 1;
    char* file_stdin;
    char* file_stdout;
    char* file_stderr;
    char* args[MAX_ARGS];
    char** arg;
} Command;

extern FILE *input_file;
extern Command cmd;

/* shell.c */
void process_input(char*);
void clear_cmd(void);
void setup_input_file(int, char**);
void setup_env_variables(void);
void cleanup(void);
void prompt(void);
void evaluate_args(char**);
void do_execute(void);
void apply_io_redirection(void);
void flip_filedescriptors(int);
void redirect_filedescriptors(void);
void restore_filedescriptors(void);


/* builtins.c */
void do_environ(char **);
void do_dir(void);
void do_clear(void);
void do_echo(void);
__attribute__ ((noreturn)) void quit(void);
void do_cd(void);
void do_pause(void);
void do_help(void);

/* signals.c */
void setup_signal_handlers(void);
void restore_signals(void);
void handle_sigchld(int);
void handle_sigtstp(int);
#endif
