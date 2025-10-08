#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "command.h"

int tokenize(const char *line, char *tokens[], int max_tokens);
void free_tokens(char *tokens[], int ntok);
int is_separator_token(const char *t);
int separateCommands(char *token[], Command command[]);

#endif //TOKENIZER_H