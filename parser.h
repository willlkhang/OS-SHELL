/**
 * @file parser.h
 * @brief Header file for command line parsing functionality
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef PARSER_H
#define PARSER_H

#include "command.h"

/**
 * @brief Parses a command line string into CommandLine structure
 * 
 * This function takes a raw command line string and parses it into
 * a structured CommandLine containing individual commands with their
 * arguments, redirections, and separators.
 * 
 * @param line The command line string to parse
 * @param cl Pointer to the CommandLine structure to populate
 * @return Number of commands parsed, or -1 on error
 */
int parse_line(const char *line, CommandLine *cl);

/**
 * @brief Searches for redirection operators in tokens
 * 
 * This function identifies and processes input/output redirection
 * operators (< and >) in the token array and sets up the appropriate
 * redirection fields in the command structure.
 * 
 * @param token Array of token strings
 * @param cp Pointer to the Command structure to update
 */
void searchRedirection(char *token[], Command *cp);

/**
 * @brief Builds the argument array for a command
 * 
 * This function constructs the argv array for a command from the
 * token array, excluding redirection operators and their operands.
 * 
 * @param token Array of token strings
 * @param cp Pointer to the Command structure to update
 */
void buildCommandArgumentArray(char *token[], Command *cp);

/**
 * @brief Frees memory allocated for parsed commands
 * 
 * This function releases all dynamically allocated memory
 * associated with the parsed command structures.
 * 
 * @param cmds Array of Command structures to free
 * @param n Number of commands in the array
 */
void free_parsed_commands(Command *cmds, int n);

#endif //PARSER_H