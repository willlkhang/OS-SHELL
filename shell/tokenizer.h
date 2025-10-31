/**
 * @file tokenizer.h
 * @brief Header file for command line tokenization functionality
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "command.h"

/**
 * @brief Tokenizes a command line string into individual tokens
 * 
 * This function parses a command line string and breaks it down into
 * individual tokens, handling quotes, escapes, and separators properly.
 * 
 * @param line The command line string to tokenize
 * @param tokens Array to store the resulting tokens
 * @param max_tokens Maximum number of tokens that can be stored
 * @return Number of tokens found, or -1 on error
 */
int tokenize(const char *line, char *tokens[], int max_tokens);

/**
 * @brief Checks if a token is a separator operator
 * 
 * This function determines whether a given token represents a shell
 * separator operator such as pipe (|), semicolon (;), or ampersand (&).
 * 
 * @param t The token string to check
 * @return 1 if the token is a separator, 0 otherwise
 */
int is_separator_token(const char *t);

/**
 * @brief Separates tokens into individual commands
 * 
 * This function takes an array of tokens and groups them into
 * individual Command structures based on separator operators.
 * 
 * @param token Array of token strings
 * @param command Array of Command structures to populate
 * @return Number of commands created
 */
int separateCommands(char *token[], Command command[]);

#endif //TOKENIZER_H