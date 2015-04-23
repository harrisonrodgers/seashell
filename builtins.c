/**
 * @file builtins.c
 * @brief Built in functions provided by the shell.
 * @author Harrison Rodgers
 * @version 1.0
 * @date 2015-04-23
 */
#include "seashell.h"

/**
 * @brief Prints the full list of environment variables to stdout.
 *
 * Supports i/o redirection.
 *
 * @param env list environment variables provided to program
 */
void do_environ(char **env)
{

    redirect_filedescriptors();

    int i = 0;
    while (env[i] != NULL) {
        printf("%s\n", env[i]);
        i++;
    }

    restore_filedescriptors();

}

/**
 * @brief Alias to the ls process, applies the -a and -l flags automatically.
 *
 * Supports i/o redirection.
 */
void do_dir()
{
    int argcount = 1;
    for (char** temp = cmd.args; *temp != NULL; temp++) {
        argcount++;
    }

    /* check args array out of bounds */
    if (argcount + 1 > MAX_ARGS) {
        fprintf(stderr, "too many arguments\n");
        return;
    }

    /* shift args elements over */
    for (int i = argcount; i > 0; i--) {
        cmd.args[i]=cmd.args[i-1];
    }

    /* add "ls" to front, replace "dir" with "-al" */
    cmd.args[0] ="ls";
    cmd.args[1] ="-al";

    do_execute();
}

/**
 * @brief Clears the terminal output by executing the clear process.
 */
void do_clear() {
    cmd.args[0] = "clear";
    cmd.args[1] = "clear";
    cmd.args[2] = NULL;
    do_execute();
}


/**
 * @brief Repeats the arguments back to the user, excluding redirection and background process
 * symbols.
 *
 * Supports i/o redirection.
 */
void do_echo() {

    redirect_filedescriptors();

    for (char** temp = cmd.args+1; *temp != NULL; temp++) {
        printf("%s ", *temp);
    }
    printf("\n");

    restore_filedescriptors();

}

/**
 * @brief Terminates the execution of the shell after cleaning up.
 */
__attribute__ ((noreturn)) void quit() {
    cleanup();
    exit(EXIT_SUCCESS);
}

/**
 * @brief Changes the current working directory if a target directory was provided, else
 * prints the full path of the current working directory.
 */
void do_cd() {
    char *path;

    char cwd[MAX_BUFFER];

    /* resolve splint 'not completely defined' warnings */
    memset(cwd, 0, sizeof(cwd));

    // display the current directory
    if (cmd.args[1] == NULL) {
        // get current working directory
        if(getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("directory name too long\n");
        }
        printf("%s\n", cwd);
        return;
    }

    // establish the exact path to change to based on input given
    if (strncmp(cmd.args[1], "~", 1) == 0) {
        // special case: home directory
        char temp_path[MAX_BUFFER];
        // replace ~ with user's home
        sprintf(temp_path, "%s", getenv("HOME"));
        // add on the rest of the provided path - if any
        if (strlen(cmd.args[1]) > 1) {
            sprintf(temp_path, "%s%s", temp_path, cmd.args[1]+1);
        }
        path = temp_path;
    } else if (strcmp(cmd.args[1], "-") == 0) {
        // special case: previous directory
        path = getenv("OLDPWD");
    } else {
        // normal case: use given directory
        path = cmd.args[1];
    }

    // change to desired directory
    if (chdir(path) < 0) {
        perror("error");
        return;
    }

    // print name of current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("error");
        return;
    }

    // update environment variables: PWD, OLDPWD
    #ifdef DEBUG
    printf("debug: setting OLDPWD=%s\n", getenv("PWD"));
    printf("debug: setting PWD=%s\n", cwd);
    #endif
    setenv("OLDPWD", getenv("PWD"), 1);
    setenv("PWD", cwd, 1); // cwd is used to process .. and . properly
}

/**
 * @brief Suspends the echo ability of the terminal, thus pausing, until newline is received.
 *
 * reference: http://man7.org/tlpi/code/online/dist/tty/no_echo.c.html
 */
void do_pause(void) {
    /* display information message */
    printf("The shell has been paused. Press 'Enter' to continue.\n");

    /* disable terminal echo ability */
    struct termios noecho, original;
    if (tcgetattr(STDIN_FILENO, &noecho) == -1) {
        perror("error - unable to retrieve terminal settings ");
        cleanup();
        exit(EXIT_FAILURE);
    }
    original = noecho; /* backup settings */
    noecho.c_lflag &= ~(unsigned int)ECHO; /* disable echo bit without altering others */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &noecho) == -1) {
        perror("error - unable to disbale terminal echo ability");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* hang until resume command is received */
    while (1) {
        int c = getchar();
        if (c == '\n' || c == EOF) {
            break;
        }
    }

    /* restore terminal echo ability */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original) == -1) {
        perror("error - unable to restore terminal echo ability");
        cleanup();
        exit(EXIT_FAILURE);
    }

}

/**
 * @brief Displays the man file to the user, via man, with less.
 *
 * Supports i/o redirection.
 */
void do_help(void) {
    /* display the man page using man with the more filter */
    char temp[MAX_BUFFER];
    cmd.args[0] = "man";
    cmd.args[1] = "-P";
    cmd.args[2] = "less";
    strncpy(temp, getenv("SHELL"), MAX_BUFFER);
    /* remove "seashell" from end */
    temp[strlen(getenv("SHELL"))-strlen("seashell")] = '\0';
    strcat(temp, "seashell.man");
    /* determine file accessibility */
    if( access(temp, R_OK ) == -1 ) {
        perror("error - unable to find man file");
        return;
    }
    //TODO: improve this
    cmd.args[3] = temp;
    cmd.args[4] = NULL;
    do_execute();
}
