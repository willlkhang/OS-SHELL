/**
 * @file command_setup.h
 * @brief Header file for command structure setup and initialisation
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef COMMAND_SETUP_H
#define COMMAND_SETUP_H

#include "command.h"

/**
 * @brief Sets up helper fields for a command structure
 * 
 * This function initialises the parser helper fields of a Command structure,
 * including first and last token indices, and separator information.
 * 
 * @param cmd Array of Command structures to modify
 * @param token Array of token strings from parsing
 * @param c Pointer to the current command counter
 * @param first Index of the first token for this command
 * @param last Index of the last token for this command
 * @param index Index of the command in the array
 */
void setup_command_helper_field(Command cmd[], char *token[], int *c, int first, int last, int index);

/**
 * @brief Sets up execution fields for a command structure
 * 
 * This function initializes the execution-related fields of a Command structure,
 * including pathname, argument count, and argument vector.
 * 
 * @param c Number of commands to process
 * @param command Array of Command structures to initialize
 * @param index Index of the command to set up
 */
void setup_command_execution_field(int c, Command command[], int index);

#endif //COMMAND_SETUP_H