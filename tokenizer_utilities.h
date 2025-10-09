#ifndef TOKENIZER_UTILITIES
#define TOKENIZER_UTILITIES

void process_quoted_string(const char **p_ptr, char *tokens[], int *ntok);
void process_escaped_character(const char **p_ptr, char *tokens[], int *ntok);
void process_control_operators(const char **p_ptr, char *tokens[], int *ntok);
void process_normal_command(const char **p_ptr, char *tokens[], int *ntok);

int is_separator_token(const char *t);

void free_tokens(char *tokens[], int ntok);

#endif // TOKENIZER_UTILITIES