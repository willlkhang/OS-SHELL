/**
 * @file utilities.h
 * @brief Header file for shell utility functions
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include "command.h"
#include <unistd.h>

#define MAX_NUM_COMMANDS  1000

/**
 * @brief Frees memory allocated for command structures
 * 
 * This function releases all dynamically allocated memory
 * associated with an array of Command structures.
 * 
 * @param cmds Array of Command structures to free
 * @param n Number of commands in the array
 */
void free_commands(Command *cmds, int n);

/**
 * @brief Applies input/output redirections for a command
 * 
 * This function sets up file redirections (stdin/stdout) for a command
 * by opening the appropriate files and redirecting file descriptors.
 * 
 * @param cmd Pointer to the Command structure with redirection information
 */
void apply_redirections(const Command *cmd);

/**
 * @brief Executes a pipeline of commands
 * 
 * This function runs a sequence of commands connected by pipes,
 * setting up the necessary pipe connections and process management.
 * 
 * @param cmds Array of Command structures in the pipeline
 * @param start Index of the first command in the pipeline
 * @param end Index of the last command in the pipeline
 * @param background Flag indicating if the pipeline should run in background
 * @return Process ID of the pipeline, or -1 on error
 */
int run_pipeline(Command *cmds, int start, int end, int background);

/**
 * @brief Executes a single command
 * 
 * This function runs a single command, handling process creation,
 * argument setup, and execution.
 * 
 * @param cmd Pointer to the Command structure to execute
 * @return Process ID of the created process, or -1 on error
 */
pid_t run_single(const Command *cmd);

#endif //UTILITIES_H