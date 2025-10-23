#ifndef WILDCARD_HANDLER_H
#define WILDCARD_HANDLER_H

int has_wildcard(const char* str);
void add_arg_to_list(char ***new_argv_ptr, int *count, int *capacity, const char *arg_to_add) ;
char** expand_wildcards(char** argv);
void free_expanded_argv(char** argv);

#endif // WILDCARD_HANDLER_H