#include "history_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup_history(history *h){
    h->head = NULL;
    h->tail = NULL;
    h->nav = NULL;
    h->history_cnt = 0;
}

void history_add(history *h, const char *cmd){
    if (!cmd || strlen(cmd) == 0) return;
    if (h->tail && strcmp(h->tail->command, cmd) == 0) return;

    HistoryNode *node = malloc(sizeof(HistoryNode));
    if (!node) { perror("malloc history node"); exit(1); }
    node->command = strdup(cmd);
    node->prev = h->tail;
    node->next = NULL;

    if (h->tail) h->tail->next = node; else h->head = node;
    h->tail = node;
    h->history_cnt++;
    h->nav = NULL;
}

void print_history(history *h){
    int idx = 1;
    for (HistoryNode *p = h->head; p != NULL; p = p->next){
        printf("%d: %s\n", idx++, p->command);
    }
}

void clean_history(history *h){
    HistoryNode *p = h->head;
    while (p){
        HistoryNode *n = p->next;
        free(p->command);
        free(p);
        p = n;
    }
    setup_history(h);
}

/**
 * @brief Expands history commands like !ls, !123, !!
 * 
 * @param line The input line that may contain history expansion
 * @param h Pointer to the history structure
 * @return Expanded line or original line if no expansion needed
 */
char* expand_history_command(const char *line, history *h) {
    if (!line || line[0] != '!') {
        return strdup(line);
    }
    
    // Handle !! (repeat last command)
    if (strcmp(line, "!!") == 0) {
        if (h->tail) {
            return strdup(h->tail->command);
        } else {
            fprintf(stderr, "history: no previous command\n");
            return strdup("");
        }
    }
    
    // Handle !n (repeat nth command)
    if (line[1] >= '0' && line[1] <= '9') {
        int cmd_num = atoi(line + 1);
        if (cmd_num > 0 && cmd_num <= h->history_cnt) {
            int idx = 1;
            for (HistoryNode *p = h->head; p != NULL; p = p->next, idx++){
                if (idx == cmd_num) return strdup(p->command);
            }
        } else {
            fprintf(stderr, "history: command %d not found\n", cmd_num);
            return strdup("");
        }
    }
    
    // Handle !string (repeat last command starting with string)
    const char *prefix = line + 1;
    size_t plen = strlen(prefix);
    for (HistoryNode *p = h->tail; p != NULL; p = p->prev){
        if (strncmp(p->command, prefix, plen) == 0) return strdup(p->command);
    }
    
    fprintf(stderr, "history: no command starting with '%s'\n", prefix);
    return strdup("");
}

// Free all memory used by the history struct
void history_free(history *h) {
    HistoryNode *p = h->head;
    while (p){
        HistoryNode *n = p->next;
        free(p->command);
        free(p);
        p = n;
    }
    h->head = h->tail = h->nav = NULL;
    h->history_cnt = 0;
}