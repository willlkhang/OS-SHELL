#include "signal_processor.h"

#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>

void doing_nothing_method(int signal) {
    // Ignore /  CTRL-C/ SIGINT / CTRL-Z / SIGTSTP  / CTRL-\ / SIGQUIT
    //This method means doing nothing, when it is called
}

void sigchld_processor(int signal) {
    pid_t pid;
    int status;

    //WNOHANG --> dont' block if not child's exited yet
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) { //reap to avoid accumulating zomebie processes
        printf("At status %d with %d\n", status, pid);
    }
}

void activate_signal_handlers(){

    //set up child signal process
    struct sigaction sa_chld;
    sa_chld.sa_handler = sigchld_processor;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa_chld, NULL);

    // Use sigaction for all signals to ensure SA_RESTART flag
    struct sigaction sa_ignore;
    sa_ignore.sa_handler = doing_nothing_method;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = SA_RESTART;
    
    sigaction(SIGINT, &sa_ignore, NULL);  // CTRL-C
    sigaction(SIGTSTP, &sa_ignore, NULL); // CTRL-Z
    sigaction(SIGQUIT, &sa_ignore, NULL); // CTRL-(\)
}   