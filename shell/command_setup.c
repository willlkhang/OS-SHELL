#include "command_setup.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void setup_command_helper_field(Command cmd[], char *token[], int *c, int first, int last, int index){
    cmd[*c].first = first;
    cmd[*c].last = last;
    cmd[*c].sep = strdup(token[index]);
    (*c)++;
}

void setup_command_execution_field(int c, Command command[], int index){
    int i = index;
    for (i = 0; i < c; ++i) { 
        command[i].argv = NULL;
        command[i].stdin_file = NULL;
        command[i].stdout_file = NULL;
        command[i].pathname = NULL;
        command[i].argc = 0;
    }
}