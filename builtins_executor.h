#ifndef BUILTINS_H
#define BUILTINS_H

#include "command.h"

#include <stddef.h> // size_t

int is_builtins(char *cmd);
int executing_builtins(Command *cmd);
int builtin_handler(Command *cmd, char prompt_buf[], size_t prompt_buf_len);

#endif //BUILTINS_H