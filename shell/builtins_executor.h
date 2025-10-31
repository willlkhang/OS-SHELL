/**
 * @file builtins_executor.h
 * @brief Header file for built-in command execution functionality
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "command.h"
#include "history.h"

#include <stddef.h> // size_t

/**
 * @brief Checks if a command is a built-in shell command
 * 
 * This function determines whether the given command string corresponds
 * to a built-in shell command (like cd, exit, history, etc.).
 * 
 * @param cmd The command string to check
 * @return 1 if the command is a built-in, 0 otherwise
 */
int is_builtins(char *cmd);

/**
 * @brief Handles execution of built-in shell commands
 * 
 * This function processes and executes built-in shell commands such as
 * cd, exit, history, and other shell-specific commands that don't
 * require external program execution.
 * 
 * @param cmd Pointer to the Command structure containing command details
 * @param prompt_buf Buffer for storing prompt information
 * @param prompt_buf_len Length of the prompt buffer
 * @param h Pointer to the history structure for command history management
 * @return 0 on success, non-zero on error
 */
int builtin_handler(Command *cmd, char prompt_buf[], size_t prompt_buf_len, history *h);

#endif //BUILTINS_H