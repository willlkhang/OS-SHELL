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
typedef struct HistoryNode_struct {
    char *command;                 /**< Command string */
    struct HistoryNode_struct *prev; /**< Previous node */
    struct HistoryNode_struct *next; /**< Next node */
} HistoryNode;

typedef struct history_struct{
    HistoryNode *head;             /**< Oldest command */
    HistoryNode *tail;             /**< Most recent command */
    HistoryNode *nav;              /**< Navigation cursor for UI */
    int history_cnt;               /**< Total number of commands in history */
} history;

#endif // HISTORY_H