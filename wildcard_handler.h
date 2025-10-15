#ifndef WILDCARD_HANDLER_H
#define WILDCARD_HANDLER_H

int has_wildcard(const char* str);
char** expand_wildcards(char** argv);
void free_expanded_argv(char** argv);

#endif // WILDCARD_HANDLER_H