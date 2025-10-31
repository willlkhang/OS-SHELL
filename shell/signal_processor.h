/**
 * @file signal_processor.h
 * @brief Header file for signal handling and processing
 * @author Ming Khang Nguyen, James Kuang 
 * @date 23/10/2025
 */

#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

/**
 * @brief Default signal handler that does nothing
 * 
 * This function serves as a placeholder signal handler that
 * performs no action when a signal is received. Used for
 * signals that should be ignored or handled minimally.
 * 
 * @param signal The signal number that was received
 */
void doing_nothing_method(int signal);

/**
 * @brief Handles SIGCHLD signals for child process management
 * 
 * This function processes SIGCHLD signals to clean up zombie
 * child processes and update the shell's process tracking.
 * 
 * @param signal The SIGCHLD signal number
 */
void sigchld_processor(int signal);

/**
 * @brief Activates all signal handlers for the shell
 * 
 * This function sets up all necessary signal handlers for
 * proper shell operation, including SIGCHLD, SIGINT, and
 * other relevant signals.
 */
void activate_signal_handlers();

#endif //SIGNAL_PROCESSOR_H