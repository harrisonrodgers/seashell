/**
 * @file signals.c
 * @brief Signal related functions.
 * @author Harrison Rodgers
 * @version 1.0
 * @date 2015-04-23
 */
#include "seashell.h"

/**
 * @brief Associates signals to custom handler functions.
 */
void setup_signal_handlers() {
    /* ignore SIGINT */
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("error - unable to set SIGINT handler");
        cleanup();
        exit(EXIT_FAILURE);
    }
    if (signal(SIGCHLD, handle_sigchld) == SIG_ERR) {
        perror("error - unable to set SIGCHILD handler");
        cleanup();
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, handle_sigtstp) == SIG_ERR) {
        perror("error - unable to set SIGSTP handler");
        cleanup();
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Restores signals to default handler functions.
 */
void restore_signals() {
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("error - unable to restore SIGINT handler");
        cleanup();
        exit(EXIT_FAILURE);
    }
    if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
        perror("error - unable to restore SIGCHILD handler");
        cleanup();
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
        perror("error - unable to restore SIGSTP handler");
        cleanup();
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Handler function to be executed upon receiving signal SIGCHLD.
 *
 * Receives the status from a child, allowing it to terminate completely (instead of getting
 * stuck as a zombie).
 */
void handle_sigchld() {
    int status = 0;
    /* receive status without hanging parent */
    while(waitpid(-1, &status, WNOHANG) > 0);
}

/**
 * @brief Handler function to be executed upon receiving signal SIGSTP.
 *
 * Prints a message to the user informing them this shell does not support being placed in the
 * background, nor does it support the suspension of child processes.
 */
void handle_sigtstp() {
    printf("\nCtrl+z is not enabled in this shell.\n");
}
