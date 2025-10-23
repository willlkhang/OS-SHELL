/**
 * @file history.h
 * @brief Header file defining the history data structure
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef HISTORY_H
#define HISTORY_H

/**
 * @brief Structure for managing command history
 * 
 * This structure implements a circular buffer for storing command history,
 * allowing efficient addition and retrieval of previously executed commands.
 */
typedef struct history_struct{
    char **history_log;         /**< Array of strings storing command history */
    int history_cnt;            /**< Total number of commands in history */
    int history_idx;            /**< Current index in the circular buffer */
    int capacity;               /**< Maximum capacity of the history buffer */
} history;

#endif // HISTORY_H