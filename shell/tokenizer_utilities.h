/**
 * @file tokenizer_utilities.h
 * @brief Header file for tokenizer utility functions
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef TOKENIZER_UTILITIES
#define TOKENIZER_UTILITIES

/**
 * @brief Processes quoted strings during tokenization
 * 
 * This function handles the parsing of quoted strings (both single and double quotes)
 * during the tokenization process, properly handling escaped characters within quotes.
 * 
 * @param p_ptr Pointer to the current position in the input string
 * @param tokens Array to store the resulting tokens
 * @param ntok Pointer to the current token count
 */
void process_quoted_string(const char **p_ptr, char *tokens[], int *ntok);

/**
 * @brief Processes escaped characters during tokenization
 * 
 * This function handles escaped characters (prefixed with backslash)
 * during tokenization, converting them to their literal values.
 * 
 * @param p_ptr Pointer to the current position in the input string
 * @param tokens Array to store the resulting tokens
 * @param ntok Pointer to the current token count
 */
void process_escaped_character(const char **p_ptr, char *tokens[], int *ntok);

/**
 * @brief Processes control operators during tokenization
 * 
 * This function identifies and processes shell control operators
 * such as pipes (|), semicolons (;), and ampersands (&) during tokenization.
 * 
 * @param p_ptr Pointer to the current position in the input string
 * @param tokens Array to store the resulting tokens
 * @param ntok Pointer to the current token count
 */
void process_control_operators(const char **p_ptr, char *tokens[], int *ntok);

/**
 * @brief Processes normal command tokens during tokenization
 * 
 * This function handles regular command tokens (non-quoted, non-escaped)
 * during the tokenization process.
 * 
 * @param p_ptr Pointer to the current position in the input string
 * @param tokens Array to store the resulting tokens
 * @param ntok Pointer to the current token count
 */
void process_normal_command(const char **p_ptr, char *tokens[], int *ntok);

/**
 * @brief Checks if a token is a separator
 * 
 * This function determines whether a given token represents a shell
 * separator operator (pipe, semicolon, ampersand, etc.).
 * 
 * @param t The token string to check
 * @return 1 if the token is a separator, 0 otherwise
 */
int is_separator_token(const char *t);

/**
 * @brief Frees memory allocated for tokens
 * 
 * This function releases all dynamically allocated memory
 * associated with the token array.
 * 
 * @param tokens Array of token strings to free
 * @param ntok Number of tokens in the array
 */
void free_tokens(char *tokens[], int ntok);

#endif // TOKENIZER_UTILITIES