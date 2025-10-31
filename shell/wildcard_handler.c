#include "wildcard_handler.h"

#include <glob.h>
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>

int has_wildcard(const char* str){
    return (
        (strchr(str, '*') != NULL) || (strchr(str, '?') != NULL)
    );
}

void free_expanded_argv(char **argv){
    if(!argv)
        return;
    for(int i = 0; argv[i] != NULL; i++){
        free(argv[i]);
    }
    free(argv);
}

void add_arg_to_list(char ***new_argv_ptr, int *count, int *capacity, const char *arg_to_add) {
    if (*count >= *capacity - 1) {
        *capacity *= 2;

        char **temp = realloc(*new_argv_ptr, *capacity * sizeof(char*));
        if (!temp) {
            perror("realloc");
            exit(1); 
        }
        *new_argv_ptr = temp;
    }

    (*new_argv_ptr)[*count] = strdup(arg_to_add);
    (*count)++;
}

char** expand_wildcards(char **argv){
    if(!argv || !argv[0])
        return NULL;
    
    int capacity = 100;
    int count = 0;

    char** new_argv = malloc(capacity * sizeof(char*));
    if (!new_argv) {
        perror("malloc");
        return NULL;
    }

    add_arg_to_list(&new_argv, &count, &capacity, argv[0]);

    for(int i = 1; argv[i] != NULL; i++){
        if(has_wildcard(argv[i])){
            glob_t result;
            int ret = glob(argv[i], GLOB_NOCHECK | GLOB_TILDE, NULL, &result);

            if(ret == 0){ // Add all matched paths
                for(size_t j = 0; j < result.gl_pathc; j++){
                    add_arg_to_list(&new_argv, &count, &capacity, result.gl_pathv[j]);
                }
            }
            else { //no glob match
                add_arg_to_list(&new_argv, &count, &capacity, argv[i]);
            }
            globfree(&result);
        }
        else { //no wildcard catc
            add_arg_to_list(&new_argv, &count, &capacity, argv[i]);
        }
    }

    new_argv[count] = NULL;
    return new_argv;
}