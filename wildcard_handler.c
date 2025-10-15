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

char** expand_wildcards(char **argv){
    if(!argv || !argv[0])
        return NULL;
    
    //build a new argument list, this mean is a command contains wc then make a new command
    int capacity = 100;
    char** new_argv = malloc(capacity * sizeof(char*));
    int count = 0;

    //start expanding the commnands
    new_argv[count++] = strdup(argv[0]);

    //traverse throughout the loop
    for(int i = 1; argv[i] != NULL; i++){
        if(has_wildcard(argv[i])){
            glob_t result;

            int ret = glob(argv[i], GLOB_NOCHECK | GLOB_TILDE, NULL, &result);

            if(ret == 0){
                //add all matched paths to new argc
                for(size_t j = 0; j < result.gl_pathc; j++){
                    if(count >= capacity - 1){
                        capacity *= 2;
                        new_argv = realloc(new_argv, capacity * sizeof(char*));
                    }
                    new_argv[count++] = strdup(result.gl_pathv[j]);
                }
            }
            else{
                if(count >= capacity - 1){
                    capacity *= 2;
                    new_argv = realloc(new_argv, capacity * sizeof(char*));
                }
                new_argv[count++] = strdup(argv[i]);
            }
        }
    }

    new_argv[count] = NULL;
    return new_argv;
}