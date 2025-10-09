#include "tokenizer.h"
#include "separators.h"
#include "tokenizer_utilities.h"
#include "command_setup.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// return number of token, and token array for system execution
int tokenize(const char *line, char *tokens[], int max_tokens) {
    int ntok = 0;
    const char *p = line;

    while (*p) {
        while (isspace((unsigned char)*p)) ++p;
        if (!*p) 
            break;

        if (ntok >= max_tokens - 1) //breaking limit
            break;

        if (*p == '\'' || *p == '"') { //process single slash
            process_quoted_string(&p, tokens, &ntok);

        } else if (*p == '\\') { // process double slash
            process_escaped_character(&p, tokens, &ntok);
        } 
        else if (*p == '>' || *p == '<' || *p == '|' || *p == ';' || *p == '&') {
            process_control_operators(&p, tokens, &ntok);
        } else {
            process_normal_command(&p, tokens, &ntok);
        }
    }
    tokens[ntok] = NULL;
    return ntok;
}

//free tokens
void free_tokens(char *tokens[], int ntok) {
    for (int i = 0; i < ntok; ++i) {
        free(tokens[i]);
        tokens[i] = NULL;
    }
}

int separateCommands(char *token[], Command command[]) {
    int i = 0;
    while (token[i]) ++i;
    int nTokens = i;
    if (nTokens == 0) return 0;

    // check first token can't be a separator 
    if (is_separator_token(token[0])) return -3;

    // ensure last token is a separator; if not, append ";"
    if (!is_separator_token(token[nTokens-1])) {
        token[nTokens] = strdup(seqSep);
        token[nTokens+1] = NULL;
        ++nTokens;
    }

    int first = 0;
    int c = 0;
    for (i = 0; i < nTokens; ++i) {
        if (is_separator_token(token[i])) {
            int last = i - 1;
            if (first > last) {
                // two seps continuously appears
                return -2;
            }
            setup_command_helper_field(command, token, &c, first, last, i);
            first = i + 1;
        }
    }

    // last token processed is a separator - we must not end with '|' (pipe)
    if (nTokens > 0 && strcmp(token[nTokens-1], pipeSep) == 0) 
        return -4;

    setup_command_execution_field(c, command, i);

    return c;
}