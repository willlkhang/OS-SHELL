#ifndef HISTORY_H
#define HISTORY_H

typedef struct history_struct{
    char **history_log;
    int history_cnt;
    int history_idx;
} history;

#endif // HISTORY_H