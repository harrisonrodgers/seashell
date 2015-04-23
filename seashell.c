/**
 * @file seashell.c
 * @brief Main shell file.
 * @author Harrison Rodgers
 * @version 1.0
 * @date 2015-04-23
 */
#include "seashell.h"
#include "signals.c"
#include "builtins.c"

FILE *input_file;

/* struct to hold details regarding command */
Command cmd;

/**
 * @brief Core process loop
 *
 * @param argc number of arguments provided to program
 * @param argv list of arguments provided to program
 * @param env list environment variables provided to program
 *
 * @return exit code
 */
int main (int argc, char* argv[], char** env)
{
    char raw_input[MAX_BUFFER];

    setup_signal_handlers();
    setup_input_file(argc, argv);
    setup_env_variables();

    /* loop over each command until the end of file is reached */
    while (!feof(input_file)) {
        prompt();

        /* reset the command information */
        clear_cmd();

        /* read next line from the input_file */
        if (fgets(raw_input, (int)sizeof(raw_input), input_file) != NULL) {

            /* ensure fgets received actual input */
            if (raw_input[0] == '\n') {
                continue;
            }

            /* tokenize the raw input */
            process_input(raw_input);

            /* evaluate the processed arguments */
            evaluate_args(env);

        }
        else if (ferror(input_file)) {
            // handle file errors
            perror("error - file");
            cleanup();
            return 1;
        }
    }

    cleanup();
    return 0;
}

/**
 * @brief Tokenizes input line, extracts tokens related to redirection and background execution.
 *
 * @param raw_input input string
 */
void process_input(char* raw_input)
{
    /* reset arg pointer to args */
    cmd.arg = cmd.args;
    *cmd.arg++ = strtok(raw_input, SEPARATORS); /* get command */
    char* token;
    while ((token = strtok(NULL, SEPARATORS))) {

        if (strcmp(token, "<") == 0 || strcmp(token, "0<") == 0) {
            /* redirect stdin */
            cmd.file_stdin = strtok(NULL, SEPARATORS);
            #ifdef DEBUG
            printf("debug: stdin redirection from: %s\n", cmd.file_stdin);
            #endif

        } else if (strcmp(token, ">") == 0 || strcmp(token, "1>") == 0) {
            /* redirect stdout */
            cmd.file_stdout = strtok(NULL, SEPARATORS);
            #ifdef DEBUG
            printf("debug: stdout redirection to %s with append = %d\n", cmd.file_stdout, cmd.is_stdout_append);
            #endif

        } else if (strcmp(token, ">>") == 0 || strcmp(token, "1>>") == 0) {
            /* redirect stdout and append */
            cmd.file_stdout = strtok(NULL, SEPARATORS);
            cmd.is_stdout_append = 1;
            #ifdef DEBUG
            printf("debug: stdout redirection to %s with append = %d\n", cmd.file_stdout, cmd.is_stdout_append);
            #endif

        } else if (strcmp(token, "2>") == 0 || strcmp(token, "2>") == 0) {
            /* redirect stderr */
            cmd.file_stderr = strtok(NULL, SEPARATORS);
            #ifdef DEBUG
            printf("debug: stderr redirection to %s with append = %d\n", cmd.file_stderr, cmd.is_stderr_append);
            #endif

        } else if (strcmp(token, "2>>") == 0 || strcmp(token, "2>>") == 0) {
            /* redirect stderr and append */
            cmd.file_stderr = strtok(NULL, SEPARATORS);
            cmd.is_stderr_append = 1;
            #ifdef DEBUG
            printf("debug: stderr redirection to %s with append = %d\n", cmd.file_stderr, cmd.is_stderr_append);
            #endif

        } else if (strcmp(token, "&>") == 0 || strcmp(token, ">&") == 0) {
            /* redirect stdout & stderr */
            cmd.file_stderr = strtok(NULL, SEPARATORS);
            cmd.file_stdout = cmd.file_stderr;
            #ifdef DEBUG
            printf("debug: stdout+stderr redirection to %s (%s)\n", cmd.file_stderr, cmd.file_stdout);
            #endif

        } else if (strcmp(token, "&>>") == 0) {
            /* redirect stdout & stderr and append*/
            cmd.file_stderr = strtok(NULL, SEPARATORS);
            cmd.file_stdout = cmd.file_stderr;
            cmd.is_stderr_append = 1;
            cmd.is_stdout_append = 1;
            #ifdef DEBUG
            printf("debug: stdout+stderr redirection to %s (%s) append %d (%d)\n", cmd.file_stderr, cmd.file_stdout, cmd.is_stderr_append, cmd.is_stdout_append);
            #endif

        } else {
            /* add to argument arg array */
            *cmd.arg++ = token;
        }

    }
    *cmd.arg = token; /* required due to using token in the while loop */

    /* check if & is last token in the command list */
    if (strcmp(*(cmd.arg-1), "&") == 0) {
        #ifdef DEBUG
        printf("debug: running as background\n");
        #endif
        cmd.is_background = 1;
        *(cmd.arg-1) = NULL;
    }
}

/**
 * @brief Resets the command variables for re-use.
 */
void clear_cmd()
{
    cmd.is_stderr_append = 0;
    cmd.is_stdout_append = 0;
    cmd.is_background = 0;
    cmd.file_stdin = NULL;
    cmd.file_stdout = NULL;
    cmd.file_stderr = NULL;
    cmd.args[0] = NULL;
    cmd.arg = cmd.args;
}

/**
 * @brief Establishes the file to obtain input from.
 *
 * If a batch file was provided, then that is opened as input. Else stdin is used.
 *
 * @param argc number of arguments provided to program
 * @param argv list of arguments provided to program
 */
void setup_input_file(int argc, char* argv[])
{
    /* read from stdin by default */
    input_file = stdin;

    if (argc > 2) {
        fprintf(stderr, "error - only one argument should be given\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* if argument given, use that file as the input */
    if (argc > 1) {

        /* determine file accessibility */
        if( access( argv[1], R_OK ) == -1 ) {
            perror("error - cannot open batch file");
            cleanup();
            exit(EXIT_FAILURE);
        }

        input_file = fopen(argv[1], "r");

        if (input_file == NULL) {
            perror("error - cannot open batch file");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * @brief Sets the SHELL environment variable to the location of the myshell binary.
 */
void setup_env_variables()
{
    /* NOTE: this is platform dependent - not supported on OSX */
    char buffer[MAX_BUFFER];
    /* read the symbolic link to the executable, which is located in /proc/self */
    ssize_t size = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    /* sometimes gets extra stuff, so terminate explicitly */
    buffer[size] = '\0';
    #ifdef DEBUG
    printf("debug: setting SHELL=%s\n", buffer);
    #endif
    setenv("SHELL", buffer, 1);
}


/**
 * @brief Closes a batch file if used.
 */
void cleanup()
{

    /* close file, if opened */
    if (input_file != stdin) {
        if(fclose(input_file) != 0) {
            perror("failed to close file\n");
        }
    }

    clear_cmd();

}


/**
 * @brief Evaluates the command.
 *
 * Compares the command to built-in functions, if a match is found the built-in function is
 * executed, if no match is found the command is treated as an external command to be exec'ed.
 *
 * @param env list environment variables provided to program
 */
void evaluate_args(char** env)
{
    if (cmd.args[0]) {
        if ((strcmp(cmd.args[0], "env") == 0) || strcmp(cmd.args[0], "environ") == 0) {
            do_environ(env);
        } else if (strcmp(cmd.args[0], "dir") == 0) {
            do_dir();
        } else if (strcmp(cmd.args[0], "clr") == 0) {
            do_clear();
        } else if (strcmp(cmd.args[0], "quit") == 0) {
            quit();
        } else if (strcmp(cmd.args[0], "cd") == 0) {
            do_cd();
        } else if (strcmp(cmd.args[0], "echo") == 0) {
            do_echo();
        } else if (strcmp(cmd.args[0], "pause") == 0) {
            do_pause();
        } else if (strcmp(cmd.args[0], "help") == 0) {
            do_help();
        } else {
            do_execute();
        }
    }
}


/**
 * @brief Performs the execution of external processes.
 *
 * A fork is created in which the external process is exec'ed. The child has appropriate actions
 * performed upon it including, restoration of signal handlers, application of i/o redirection ...
 *
 */
void do_execute()
{

    #ifdef DEBUG
    printf("debug: ");
    char** temp;
    int argcount = 0;
    for (temp = cmd.args; *temp != NULL; temp++) {
        printf("%d=%s ", argcount++, *temp);
    }
    printf("%d=%s", argcount, *temp++); /* show (null) */
    printf("\n");
    #endif

    int pid = fork();

    if (pid == 0) {
        restore_signals();
        apply_io_redirection();
        setenv("PARENT", getenv("SHELL"), 1);
        execvp(*cmd.args, cmd.args);
        perror("error - unable to execute external program");
        cleanup();
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // for background, simply don't wait
        if (cmd.is_background == 0) {
            int status = 0;
            pid_t wpid;
            // wait until child is finished
            while ((wpid = waitpid(pid, &status, 0)) > 0) {
                #ifdef DEBUG
                printf("debug: %d exited with %d\n", wpid, status);
                #endif
            }
        }
    } else {
        perror("error - unable to execute external program");
    }

}

/**
 * @brief Performs the appropriate i/o redirections.
 *
 * Opens specified files and associates corresponding standard text streams to these files.
 */
void apply_io_redirection()
{
    /* input redirection */
    if (cmd.file_stdin != NULL) {
       if (freopen(cmd.file_stdin, "r", stdin) == NULL) {
            perror("error - failed to redirect stdin");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }

    /* output redirection (stdout) */
    if (cmd.file_stdout != NULL) {
        if (cmd.is_stdout_append) {
            if (freopen(cmd.file_stdout, "a", stdout) == NULL) {
                perror("error - failed to redirect stdout");
                cleanup();
                exit(EXIT_FAILURE);
            }
        } else {
            if (freopen(cmd.file_stdout, "w", stdout) == NULL) {
                perror("error - failed to redirect stdout");
                cleanup();
                exit(EXIT_FAILURE);
            }
        }
    }

    /* output redirection (stderr) */
    if (cmd.file_stderr != NULL) {
        if (cmd.is_stderr_append) {
            if (freopen(cmd.file_stderr, "a", stderr) == NULL) {
                perror("error - failed to redirect stderr");
                cleanup();
                exit(EXIT_FAILURE);
            }
        } else {
            if (freopen(cmd.file_stderr, "w", stderr) == NULL) {
                perror("error - failed to redirect stderr");
                cleanup();
                exit(EXIT_FAILURE);
            }
        }
    }

}

/**
 * @brief Applies temporary i/o redirection, to be applied on built-in functions.
 */
void redirect_filedescriptors()
{
    flip_filedescriptors(1);
}

/**
 * @brief Restores original i/o standard text streams to values before redirection was performed.
 */
void restore_filedescriptors()
{
    flip_filedescriptors(0);
}

/**
 * @brief Toggles the i/o redirection, to be applied on built-in functions.
 *
 * @param action specifies the action to be performed, 1 = redirect i/o, 0 = restore i/o
 */
void flip_filedescriptors(int action)
{
    static int backup_stdin = 0;
    static int new_stdin = 0;
    static int backup_stdout = 0;
    static int new_stdout = 0;
    static int backup_stderr = 0;
    static int new_stderr = 0;

    /* redirect file descriptors */
    if (action == 1) {
        if (cmd.file_stdin != NULL) {
            new_stdin = open(cmd.file_stdin, O_RDONLY);
            backup_stdin = dup(fileno(stdin));
            dup2(new_stdin, fileno(stdin));
        }
        if (cmd.file_stdout != NULL) {
            if (cmd.is_stdout_append) {
                new_stdout = open(cmd.file_stdout, O_WRONLY|O_CREAT|O_APPEND, 0644);
            } else {
                new_stdout = open(cmd.file_stdout, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            }
            backup_stdout = dup(fileno(stdout));
            dup2(new_stdout, fileno(stdout));
        }
        if (cmd.file_stderr != NULL) {
            if (cmd.is_stderr_append) {
                new_stderr = open(cmd.file_stderr, O_WRONLY|O_CREAT|O_APPEND, 0644);
            } else {
                new_stderr = open(cmd.file_stderr, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            }
            backup_stderr = dup(fileno(stderr));
            dup2(new_stderr, fileno(stderr));
        }

    /* restore file descriptors */
    } else {
        if (cmd.file_stdin != NULL) {
            dup2(backup_stdin, fileno(stdin));
            close(new_stdin);
        }
        if (cmd.file_stdout != NULL) {
            dup2(backup_stdout, fileno(stdout));
            close(new_stdout);
        }
        if (cmd.file_stderr != NULL) {
            dup2(backup_stderr, fileno(stderr));
            close(new_stderr);
        }
    }

}

/**
 * @brief Displays a prompt message to the standard output.
 *
 * The prompt contains the current working directory. The prompt is only displayed to terminals
 * and is not printed when processing batch files.
 */
void prompt(void)
{
    /* establish if stdin is really a terminal or the result of file redirection */
    if (isatty(fileno(input_file))) {
        printf("%s $ ", getenv("PWD"));
        fflush(stdout);
    }
}
