#ifndef PARSER_H
#define PARSER_H

#include "command.h"

int parse_line(const char *line, CommandLine *cl);

void searchRedirection(char *token[], Command *cp);
void buildCommandArgumentArray(char *token[], Command *cp);
void free_parsed_commands(Command *cmds, int n);
#endif //PARSER_H