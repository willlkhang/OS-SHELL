#ifndef COMMAND_SETUP_H
#define COMMAND_SETUP_H

#include "command.h"

void setup_command_helper_field(Command cmd[], char *token[], int *c, int first, int last, int index);
void setup_command_execution_field(int c, Command command[], int index);

#endif //COMMAND_SETUP_H