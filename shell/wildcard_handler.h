/**
 * @file wildcard_handler.h
 * @brief Header file for wildcard pattern expansion functionality
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef WILDCARD_HANDLER_H
#define WILDCARD_HANDLER_H

/**
 * @brief Checks if a string contains wildcard characters
 * 
 * This function determines whether a given string contains
 * wildcard characters (* or ?) that need expansion.
 * 
 * @param str The string to check for wildcards
 * @return 1 if wildcards are found, 0 otherwise
 */
int has_wildcard(const char* str);

/**
 * @brief Adds an argument to a dynamically growing argument list
 * 
 * This function adds a new argument to an existing argument list,
 * automatically expanding the list capacity as needed.
 * 
 * @param new_argv_ptr Pointer to the argument list array
 * @param count Pointer to the current count of arguments
 * @param capacity Pointer to the current capacity of the list
 * @param arg_to_add The argument string to add
 */
void add_arg_to_list(char ***new_argv_ptr, int *count, int *capacity, const char *arg_to_add);

/**
 * @brief Expands wildcard patterns in argument array
 * 
 * This function processes an array of command arguments and expands
 * any wildcard patterns (* and ?) to match actual files in the filesystem.
 * 
 * @param argv Array of argument strings to process
 * @return New array of expanded arguments, or NULL on error
 */
char** expand_wildcards(char** argv);

/**
 * @brief Frees memory allocated for expanded argument array
 * 
 * This function releases all dynamically allocated memory
 * associated with an expanded argument array created by expand_wildcards.
 * 
 * @param argv Array of expanded argument strings to free
 */
void free_expanded_argv(char** argv);

#endif // WILDCARD_HANDLER_H