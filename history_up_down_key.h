#ifndef HISTORY_UP_DOWN_KEY_H
#define HISTORY_UP_DOWN_KEY_H

#include "history.h"

char *history_key_up(history *h);
char *history_key_down(history * h);

int get_line_with_history(char *line, int size, history *h);

#endif // HISTORY_UP_DOWN_KEY_H