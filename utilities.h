#ifndef UTILITIES_H
#define UTILITIES_H

#include "command.h"
#include <unistd.h>

#define MAX_NUM_COMMANDS  1000

void free_commands(Command *cmds, int n);
void apply_redirections(const Command *cmd);
int run_pipeline(Command *cmds, int start, int end, int background);
pid_t run_single(const Command *cmd);


#endif //UTILITIES_H