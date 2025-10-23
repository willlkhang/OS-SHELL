#ifndef HISTORY_H
#define HISTORY_H

typedef struct history_struct{
    char **history_log; // use different data structure to stack or linked list
    int history_cnt;
    int history_idx;
    int capacity;
} history;

#endif // HISTORY_H