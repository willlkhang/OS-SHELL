#ifndef HISTORY_PROCESS_H
#define HISTORY_PROCESS_H

#include "history.h"

#define MAX_HISTORY_log 999

void setup_history(history *h);
void history_add(history *h, const char *cmd);
void print_history(history *h);
void history_free(history *h);

#endif // HISTORY_PROCESS_H