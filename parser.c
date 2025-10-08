/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "command.h"
#include "separators.h"
#include "tokenizer.h"


/* searchRedirection and buildCommandArgumentArray implementations
 * similar to your functions but robust to token memory (we use strdup earlier)
 */
void searchRedirection(char *token[], Command *cp) {
    for (int i = cp->first; i <= cp->last; ++i) {
        if (strcmp(token[i], "<") == 0) {
            if (i+1 <= cp->last) {
                cp->stdin_file = strdup(token[i+1]);
                ++i;
            }
        } else if (strcmp(token[i], ">") == 0) {
            if (i+1 <= cp->last) {
                cp->stdout_file = strdup(token[i+1]);
                ++i;
            }
        } else if (strcmp(token[i], "2>") == 0) {
            /* If you want to support stderr redirection, add field or reuse stdout_file for now */
            if (i+1 <= cp->last) {
                /* For now we can ignore or treat specially; you can add stderr_file in struct */
                ++i;
            }
        }
    }
}

void buildCommandArgumentArray(char *token[], Command *cp) {
    int max_args = (cp->last - cp->first + 1) + 1;
    cp->argv = malloc(sizeof(char *) * max_args);
    if (!cp->argv) { perror("malloc"); exit(1); }

    int k = 0;
    for (int i = cp->first; i <= cp->last; ++i) {
        if (strcmp(token[i], "<") == 0 || strcmp(token[i], ">") == 0 || strcmp(token[i], "2>") == 0) {
            ++i; /* skip filename token */
            continue;
        }
        cp->argv[k++] = strdup(token[i]); /* keep copy */
    }
    cp->argv[k] = NULL;
    cp->argc = k;
    if (k > 0) cp->pathname = strdup(cp->argv[0]); else cp->pathname = NULL;
}

/* free allocated internals in commands produced by parser */
void free_parsed_commands(Command *cmds, int n) {
    for (int i = 0; i < n; ++i) {
        if (cmds[i].sep) { 
            free(cmds[i].sep); 
            cmds[i].sep = NULL;
        }
        if (cmds[i].pathname) { 
            free(cmds[i].pathname); 
            cmds[i].pathname = NULL; 
        }
        if (cmds[i].stdin_file) { 
            free(cmds[i].stdin_file); 
            cmds[i].stdin_file = NULL; 
        }
        if (cmds[i].stdout_file) { 
            free(cmds[i].stdout_file); 
            cmds[i].stdout_file = NULL; 
        }
        if (cmds[i].argv) {
            for (int j = 0; j < cmds[i].argc; ++j) 
                free(cmds[i].argv[j]);
            free(cmds[i].argv);
            cmds[i].argv = NULL;
        }
    }
}

/* High-level parse: tokenizes line, calls separateCommands, fills in argv/redir and sets CommandLine.
 * Returns number of commands or negative for parse error.
 * Caller must call free_parsed_commands(cmdline->commands, returned_count) to free memory.
 */
int parse_line(const char *line, CommandLine *cl) {
    char *tokens[1024];
    for (int i = 0; i < 1024; ++i) tokens[i] = NULL;
    int ntok = tokenize(line, tokens, 1024);

    /* convert tokens[] to modifiable array for separateCommands (it expects char*[] ownership) */
    /* tokens are strdup'd already so it's OK */
    int ncmds = separateCommands(tokens, cl->commands);
    if (ncmds <= 0) { free_tokens(tokens, ntok); return ncmds; }

    /* for each command, search redirections and build argv */
    for (int i = 0; i < ncmds; ++i) {
        searchRedirection(tokens, &cl->commands[i]);
        buildCommandArgumentArray(tokens, &cl->commands[i]);
    }

    cl->amount = ncmds;
    /* background: if last command's separator is & */
    if (ncmds > 0 && cl->commands[ncmds-1].sep && strcmp(cl->commands[ncmds-1].sep, conSep) == 0)
        cl->background = 1;
    else
        cl->background = 0;

    free_tokens(tokens, ntok);
    return ncmds;
}
