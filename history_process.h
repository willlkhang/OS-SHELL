/**
 * @file history_process.h
 * @brief Header file for command history processing and management
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef HISTORY_PROCESS_H
#define HISTORY_PROCESS_H

#include "history.h"

#define MAX_HISTORY_log 999

/**
 * @brief Initialises the history structure
 * 
 * This function sets up the history structure with initial values
 * and allocates necessary memory for storing command history.
 * 
 * @param h Pointer to the history structure to initialize
 */
void setup_history(history *h);

/**
 * @brief Adds a command to the history
 * 
 * This function adds a new command string to the command history,
 * managing the circular buffer and updating the history index.
 * 
 * @param h Pointer to the history structure
 * @param cmd Command string to add to history
 */
void history_add(history *h, const char *cmd);

/**
 * @brief Prints the command history
 * 
 * This function displays all commands stored in the history,
 * typically with line numbers for reference.
 * 
 * @param h Pointer to the history structure
 */
void print_history(history *h);

/**
 * @brief Cleans up the history structure
 * 
 * This function frees all memory associated with the history
 * structure and resets it to an empty state.
 * 
 * @param h Pointer to the history structure to clean
 */
void clean_history(history *h);

/**
 * @brief Frees all memory associated with the history structure
 * 
 * This function performs a complete cleanup of the history structure,
 * freeing all dynamically allocated memory.
 * 
 * @param h Pointer to the history structure to free
 */
void history_free(history *h);

#endif // HISTORY_PROCESS_H