/**
 * @file command.h
 * @brief Header file defining command structures and data types
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef COMMAND_H
#define COMMAND_H

#define MAX_COMMANDS 200

/**
 * @brief Structure representing a single command with its execution context
 * 
 * This structure contains all the information needed to execute a command,
 * including parser helper fields, execution parameters, and I/O redirection.
 */
typedef struct Command_struct
{   
    //parser helper
    int first;                 /**< Index of first token in the command */
    int last;                  /**< Index of last token in the command */
    char *sep;                 /**< Separator character (wildcard) */

    //execution
    char *pathname;         /**< Full path to the executable */
    int argc;               /**< Number of arguments */
    char **argv;            /**< Array of argument strings */
    char *stdin_file;       /**< File name for stdin redirection (NULL if not redirected) */
    char *stdout_file;      /**< File name for stdout redirection (NULL if not redirected) */
    char *stderr_file;      /**< File name for stderr redirection (NULL if not redirected) */
    char suffix;            /**< Command suffix: ' ', '&', ';', '|' */
} 
Command;

/**
 * @brief Structure representing a command line with multiple commands
 * 
 * This structure contains an array of commands and metadata about
 * the command line execution context.
 */
typedef struct Command_line_Strucr
{
    Command commands[MAX_COMMANDS]; /**< Array of commands in the command line */
    int amount; /**< Number of commands in the command line */
    int background; /**< Flag indicating if the command line should run in background */
}
CommandLine;

#endif