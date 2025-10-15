#include "history_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup_history(history *h){
    h->history_idx = 0;
    h->history_cnt = 0;
    h->capacity = MAX_HISTORY_log;
    h->history_log = malloc(h->capacity * sizeof(char *));
}

void history_add(history *h, const char *cmd){
    // Don't add empty commands or duplicates of the last command
    if (strlen(cmd) == 0 || (h->history_cnt > 0 && strcmp(h->history_log[h->history_cnt - 1], cmd) == 0)) {
        return;
    }

    // If we've reached capacity, double it
    if (h->history_cnt >= h->capacity) {
        h->capacity *= 2;
        h->history_log = realloc(h->history_log, h->capacity * sizeof(char *));
    }

    // Add the new command
    h->history_log[h->history_cnt] = strdup(cmd);
    h->history_cnt++;
    h->history_idx = h->history_cnt; // Reset index to the end
}

void print_history(history *h){
    for (int i = 0; i < h->history_cnt; i++) {
        // Print with a number, starting from 1 for readability
        printf("%%: %s\n", h->history_log[i]);
    }
}

void clean_history(history *h){
    for (int i = 0; i < h->history_cnt; i++){
        free(h->history_log[i]);
    }
    free(h->history_log);

    setup_history(h);
}

// Free all memory used by the history struct
void history_free(history *h) {
    for (int i = 0; i < h->history_cnt; i++) {
        free(h->history_log[i]);
    }
    free(h->history_log);
}