#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#include "command_executor.h"
#include "utilities.h"
#include "builtins_executor.h"
#include "history_process.h"

#define MAX_BUF_LEN 524

int prompt_eater(char **line, char *promptbuffer, size_t *len, ssize_t *nread){
    //display prompt
    printf("%s", promptbuffer);
    fflush(stdout);

    *nread = getline(line, len, stdin);
    if (*nread == -1) { //EOF or error
        printf("\n");
        return 0;
    }

    //strip trailing newline 
    if (*nread > 0 && (*line)[*nread-1] == '\n') 
        (*line)[*nread-1] = '\0';

    //skip empty line
    if (*line[0] == '\0') return 1;

}

int parser_erorr(int ncmd){
    if (ncmd < 0) {
        switch (ncmd) {
            case -2: 
                fprintf(stderr, "parse error: two separators in a row\n"); 
                return 0;
            case -3: 
                fprintf(stderr, "parse error: first token is a separator\n"); 
                return 0;
            case -4: 
                fprintf(stderr, "parse error: line ends with a pipe '|'\n"); 
                    return 0;
            default: 
                fprintf(stderr, "parse error: code %d\n", ncmd);
        }
        return 1;
        }
    if (ncmd == 0) return 1; // dont not anything
}

void run_background(Command *c){
    int background = (c->sep && strcmp(c->sep, "&") == 0);
    pid_t pid = run_single(c);
    // wait for background
    if (pid > 0 && !background) {
        int status;
        waitpid(pid, &status, 0);
    }
}

void execute_commads(int *ncmd_ptr, CommandLine *cl, char *promptbuffer, history *h){
    int ncmd = *ncmd_ptr;

    for (int i = 0; i < ncmd; ) {
        //detect pipe
        Command *curr_commamd = &cl->commands[i];
        if (curr_commamd->sep && strcmp(curr_commamd->sep, "|") == 0) {
        
            //find next sep which is not pipe
            //for example cmd1 | cmd2 | cmd 3 &
            int j = i;
            while ((j < ncmd - 1) && (cl->commands[j].sep) && (strcmp(cl->commands[j].sep, "|") == 0)) 
                j++;

            // operate background with final sep is &
            int background = 0;
            if (cl->commands[j].sep && strcmp(cl->commands[j].sep, "&") == 0) 
                background = 1;

            // run pip using lab's methods
            if (run_pipeline(cl->commands, i, j, background) < 0) {
                fprintf(stderr, "pipeline execution error\n");
            }

            i = j + 1;
            continue;
        }

        // if there is not pipe detected
        Command *c = curr_commamd;

        // const char *history_c = c->argv[0];
        // history_add(h, history_c);

        //run builtins
        if (builtin_handler(c, promptbuffer, MAX_BUF_LEN, h)) {
            //if there is builtins, the method will execute, and increase the index stream
            i++;
            continue;
        }

        run_background(c);

        i++;
    }
}