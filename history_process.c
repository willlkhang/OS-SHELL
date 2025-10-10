#include "history_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HISTORY_log 999

char *history[MAX_HISTORY_log];
int history_cnt = 0;
int history_idx = 0;

void add_cmd_to_history(char *cmd){
    if(history[history_idx] != NULL){
        free(history[history_idx]);
    }

    history[history_idx] = strdup(cmd); //deep copy string
    
    if(history[history_idx] == NULL){
        perror("strdup error");
        //exit (1); 
    }
    history_idx = (history_idx+1)%MAX_HISTORY_log; // avoid exceeding the limit

    if(history_idx < MAX_HISTORY_log)
        history_cnt++;
}

void print_history(){
    printf("Command history log:\n");
    
    int start_idx = (history_cnt < MAX_HISTORY_log) ? 0 : history_idx;

    for(int i = 0; i < history_cnt; i++){
        int curr_idx = (start_idx+1)%MAX_HISTORY_log;
        printf("%d: %s\n", i + 1, history[curr_idx]);
    }
}

void clean_history(){ //deep release
    for(int i = 0; i < MAX_HISTORY_log; i++){
        if(history[i] != NULL){
            free(history[i]);
            history[i] = NULL;
        }
    }
}