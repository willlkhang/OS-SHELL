#ifndef BUILTINS_H
#define BUILTINS_H

#include "command.h"
#include "history.h"

#include <stddef.h> // size_t

int is_builtins(char *cmd);
int builtin_handler(Command *cmd, char prompt_buf[], size_t prompt_buf_len, history *h);

#endif //BUILTINS_H