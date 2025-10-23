/**
 * @file command_executor.h
 * @brief Header file for command execution and process management 
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include "command.h"
#include "history.h"

#include <stddef.h>
#include <unistd.h>

/**
 * @brief Reads input from the user prompt
 * 
 * This function handles reading user input from the command line,
 * managing the prompt buffer and handling input length.
 * 
 * @param line Pointer to the line buffer
 * @param promptbuffer Buffer for storing the prompt
 * @param len Pointer to the length of the input
 * @param nread Pointer to the number of bytes read
 * @return 0 on success, non-zero on error
 */
int prompt_eater(char **line, char *promptbuffer, size_t *len, ssize_t *nread);

/**
 * @brief Handles parser errors
 * 
 * This function processes and reports errors that occur during
 * command parsing.
 * 
 * @param ncmd Number of commands that caused the error
 * @return Error code indicating the type of parser error
 */
int parser_erorr(int ncmd);

/**
 * @brief Executes a command in the background
 * 
 * This function runs a command as a background process,
 * typically indicated by the '&' operator.
 * 
 * @param c Pointer to the Command structure to execute
 */
void run_background(Command *c);

/**
 * @brief Executes a sequence of commands
 * 
 * This function processes and executes a command line containing
 * multiple commands, handling pipes, redirections, and background
 * execution as needed.
 * 
 * @param ncmd_ptr Pointer to the number of commands
 * @param cl Pointer to the CommandLine structure containing commands
 * @param promptbuffer Buffer containing the prompt information
 * @param h Pointer to the history structure
 */
void execute_commads(int *ncmd_ptr, CommandLine *cl, char *promptbuffer, history *h);

#endif //COMMAND_EXECUTOR_H