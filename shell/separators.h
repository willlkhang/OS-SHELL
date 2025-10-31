/**
 * @file separators.h
 * @brief Header file defining command separator constants
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef SEPARATORS_H
#define SEPARATORS_H

//extern variable cuz I declared them outside .c file
/**
 * @brief Pipe separator constant
 * 
 * This constant represents the pipe operator (|) used for
 * connecting the output of one command to the input of another.
 */
extern const char *pipeSep;

/**
 * @brief Sequential separator constant
 * 
 * This constant represents the semicolon (;) used for
 * executing commands sequentially.
 */
extern const char *seqSep;

/**
 * @brief Conditional separator constant
 * 
 * This constant represents the conditional operator (&& or ||)
 * used for conditional command execution.
 */
extern const char *conSep;

#endif //SEPARATORS_H