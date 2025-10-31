/**
 * @file interactive_input.h
 * @brief Header file for interactive input with arrow key navigation
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef INTERACTIVE_INPUT_H
#define INTERACTIVE_INPUT_H

#include "history.h"

/**
 * @brief Reads input with arrow key navigation support
 * 
 * This function provides interactive input with history navigation
 * using arrow keys, similar to bash.
 * 
 * @param prompt The prompt string to display
 * @param h Pointer to the history structure
 * @return The input line (must be freed by caller)
 */
char* get_interactive_input(const char *prompt, history *h);

/**
 * @brief Initializes interactive input mode
 * 
 * This function sets up the terminal for interactive input
 * with arrow key support.
 */
void init_interactive_input(void);

/**
 * @brief Cleans up interactive input mode
 * 
 * This function restores the terminal to its original state.
 */
void cleanup_interactive_input(void);

#endif // INTERACTIVE_INPUT_H
