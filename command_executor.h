#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include "command.h"
#include "history.h"

#include <stddef.h>
#include <unistd.h>

int prompt_eater(char **line, char *promptbuffer, size_t *len, ssize_t *nread);
int parser_erorr(int ncmd);
void run_background(Command *c);
void execute_commads(int *ncmd_ptr, CommandLine *cl, char *promptbuffer, history *h);

#endif //COMMAND_EXECUTOR_H