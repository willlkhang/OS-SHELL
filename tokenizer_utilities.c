#include "tokenizer_utilities.h"
#include "separators.h"

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

int is_separator_token(const char *t) {
    if (!t) return 0;
    return (strcmp(t, pipeSep) == 0) || (strcmp(t, seqSep) == 0) || (strcmp(t, conSep) == 0);
}