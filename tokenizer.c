#include "tokenizer.h"
#include "separators.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void process_quoted_string(const char **p_ptr, char *tokens[], int *ntok){
    const char *p = *p_ptr;
    char quote = *p++;
    const char *start = p;
    char buf[4096];
    int bi = 0;
    
    while (*p && *p != quote) {
        if (*p == '\\' && quote == '"' && *(p+1) != '\0') {
            ++p;

            buf[bi++] = *p ? *p : '\\';
            if (*p) ++p;

        } else {
            buf[bi++] = *p++;
        }
        if (bi >= (int)sizeof(buf)-1) 
            break;
    }

    buf[bi] = '\0';
    tokens[(*ntok)++] = strdup(buf);
    if (*p == quote) ++p;

    *p_ptr = p;
}

void process_escaped_character(const char **p_ptr, char *tokens[], int *ntok){
    const char *p = *p_ptr;
    ++p;
    char tmp[2] = { *p ? *p : '\\', '\0' };

    tokens[(*ntok)++] = strdup(tmp);
    if (*p) ++p;

    *p_ptr = p;
}

void process_control_operators(const char **p_ptr, char *tokens[], int *ntok){
    //handle 2> separately 
    const char *p = *p_ptr;
    if (*p == '>' && *(p+1) == '>') { //support >> if you want (append) 
        tokens[(*ntok)++] = strdup(">>");
        p += 2;
    } else if (*p == '2' && *(p+1) == '>' ) {
        //handle "2>" (error redirection)
        tokens[(*ntok)++] = strdup("2>");
        p += 2;
    } else {
        char s[2] = { *p, '\0' };
        tokens[(*ntok)++] = strdup(s);
        ++p;
    }

    *p_ptr = p;
}

void process_normal_command(const char **p_ptr, char *tokens[], int *ntok){
    const char *p = *p_ptr;
    //normal token until space or special char
    char buf[4096];
    int bi = 0;
    while (*p && !isspace((unsigned char)*p) && *p!='>' && *p!='<' && *p!='|' && *p!=';' && *p!='&') {
        if (*p == '\\' && *(p+1) != '\0') {
            ++p;
            buf[bi++] = *p++;
        } else {
            buf[bi++] = *p++;
        }

        if (bi >= (int)sizeof(buf)-1) break;
    }
    buf[bi] = '\0';
    tokens[(*ntok)++] = strdup(buf);
    *p_ptr = p;
}

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
            // char buf[4096];
            // int bi = 0;
            // while (*p && !isspace((unsigned char)*p) && *p!='>' && *p!='<' && *p!='|' && *p!=';' && *p!='&') {
            //     if (*p == '\\' && *(p+1) != '\0') {
            //         ++p;
            //         buf[bi++] = *p++;
            //     } else {
            //         buf[bi++] = *p++;
            //     }
            //     if (bi >= (int)sizeof(buf)-1) break;
            // }
            // buf[bi] = '\0';
            // tokens[ntok++] = strdup(buf);
        }
    }
    tokens[ntok] = NULL;
    return ntok;
}

/* free tokens returned by tokenize */
void free_tokens(char *tokens[], int ntok) {
    for (int i = 0; i < ntok; ++i) {
        free(tokens[i]);
        tokens[i] = NULL;
    }
}

/* helper: check if token equals one of the separators */
int is_separator_token(const char *t) {
    if (!t) return 0;
    return (strcmp(t, pipeSep) == 0) || (strcmp(t, seqSep) == 0) || (strcmp(t, conSep) == 0);
}

int separateCommands(char *token[], Command command[]) {
    int i = 0;
    while (token[i]) ++i;
    int nTokens = i;
    if (nTokens == 0) return 0;

    /* check first token can't be a separator */
    if (is_separator_token(token[0])) return -3;

    /* ensure last token is a separator; if not, append ";" */
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
                /* two separators in a row */
                return -2;
            }
            command[c].first = first;
            command[c].last  = last;
            command[c].sep   = strdup(token[i]); /* remember to free later */
            ++c;
            first = i + 1;
        }
    }

    /* last token processed is a separator - we must not end with '|' (pipe) */
    if (nTokens > 0 && strcmp(token[nTokens-1], pipeSep) == 0) return -4;

    /* initialize rest of fields lightly; caller will handle redirections and argv building */
    for (i = 0; i < c; ++i) {
        command[i].argv = NULL;
        command[i].stdin_file = NULL;
        command[i].stdout_file = NULL;
        command[i].pathname = NULL;
        command[i].argc = 0;
    }

    return c;
}