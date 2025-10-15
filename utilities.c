#include "utilities.h"
#include "command.h"

#include "wildcard_handler.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

void free_commands(Command *cmds, int n){
    for (int i = 0; i < n; i++){
        free(cmds[i].argv);
        cmds[i].argv = NULL;
    }
}

void apply_redirections(const Command *cmd){
    if (cmd->stdin_file){
        int fd = open(cmd->stdin_file, O_RDONLY);
        if (fd < 0){ 
            perror("open <"); 
            exit(1); 
        }
        if (dup2(fd, STDIN_FILENO) < 0){ 
            perror("dup2 <"); 
            exit(1); 
        }
        close(fd);
    }
    if (cmd->stdout_file){
        int fd = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0){ 
            perror("open >"); 
            exit(1); 
        }
        if (dup2(fd, STDOUT_FILENO) < 0){ 
            perror("dup2 >"); 
            exit(1); }
        close(fd);
    }
}

pid_t run_single(const Command *cmd){
    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        return -1;
    }
    if (pid == 0){
        apply_redirections(cmd);

        char **expanded_argv = expand_wildcards(cmd->argv);
        if (expanded_argv == NULL || expanded_argv[0] == NULL) {
            exit(0); 
        }
        execvp(expanded_argv[0], expanded_argv);
        perror("execvp");
        free_expanded_argv(expanded_argv);
        exit(1);
    }
    return pid;
}

int run_pipeline(Command *cmds, int start, int end, int background){
    int n = end - start + 1;
    int pipes_needed = n - 1;
    int fds[2 * (MAX_NUM_COMMANDS)];
    int pipe_idx = 0;

    for (int i = 0; i < pipes_needed; i++){
        if (pipe(&fds[2*i]) < 0){
            perror("pipe");
            return -1;
        }
    }

    pid_t pids[MAX_NUM_COMMANDS];
    for (int i = 0; i < n; i++){
        pid_t pid = fork();
        if (pid < 0){
            perror("fork");
            return -1;
        }
        if (pid == 0){
            // hook stdin from previous pipe if not first
            if (i > 0){
                if (dup2(fds[2*(i-1)], STDIN_FILENO) < 0){ 
                    perror("dup2 stdin"); 
                    exit(1); 
                }
            }
            // hook stdout to next pipe if not last
            if (i < n - 1){
                if (dup2(fds[2*i + 1], STDOUT_FILENO) < 0){ 
                    perror("dup2 stdout"); 
                    exit(1); 
                }
            }

            // close all pipe fds in child
            for (int k = 0; k < 2*pipes_needed; k++) 
                close(fds[k]);

            // per-command < and > (will override pipe ends if specified)
            apply_redirections(&cmds[start + i]);

            char** expand_argv = expand_wildcards(cmds[start + i].argv);
            if(expand_argv == NULL || expand_argv[0] == NULL){
                exit(0);
            }
            execvp(expand_argv[0], expand_argv);

            perror("execvp");

            free_expanded_argv(expand_argv);
            exit(1);
        }
        pids[i] = pid;
    }

    // parent closes all pipe fds
    for (int k = 0; k < 2*pipes_needed; k++) 
        close(fds[k]);

    // wait for all in the pipeline unless background
    if (!background){
        int status;
        for (int i = 0; i < n; i++) 
            waitpid(pids[i], &status, 0);
    }
    return 0;
}
