#include "interactive_input.h"
#include "history_process.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

// Terminal state management
static struct termios original_termios;
static int terminal_modified = 0;

// Signal handler to ensure proper cleanup
static void cleanup_signal_handler(int sig) {
    cleanup_interactive_input();
    exit(0);
}

void init_interactive_input(void) {
    // Save original terminal state
    tcgetattr(STDIN_FILENO, &original_termios);
    
    // Set up signal handlers for proper cleanup
    signal(SIGINT, cleanup_signal_handler);
    signal(SIGTERM, cleanup_signal_handler);
    signal(SIGQUIT, cleanup_signal_handler);
    
    terminal_modified = 0; // Don't modify terminal globally
}

void cleanup_interactive_input(void) {
    if (terminal_modified) {
        // Force a newline before restoring terminal
        printf("\n");
        fflush(stdout);
        
        // Restore original terminal state
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
        
        // Ensure terminal is in a clean state
        printf("\033[0m"); // Reset all terminal attributes
        fflush(stdout);
        
        // Force terminal to flush any pending input
        tcflush(STDIN_FILENO, TCIOFLUSH);
        
        terminal_modified = 0;
    }
}

char* get_interactive_input(const char *prompt, history *h) {
    char line[4096] = {0};
    int pos = 0;
    int ch;
    
    // Set terminal to raw mode only during input
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
    printf("%s", prompt);
    fflush(stdout);
    
    while ((ch = getchar()) != '\n' && ch != '\r') {
        if (ch == 27) { // ESC sequence (arrow keys)
            int ch2 = getchar();
            if (ch2 == '[') {
                int ch3 = getchar();
                switch (ch3) {
                    case 'A': // Up arrow
                        if (h && h->history_cnt > 0) {
                            if (h->nav == NULL) {
                                h->nav = h->tail;
                            } else if (h->nav->prev) {
                                h->nav = h->nav->prev;
                            }
                            
                            // Clear current line and show history
                            printf("\r%s", prompt);
                            for (int i = 0; i < pos; i++) printf(" ");
                            printf("\r%s", prompt);
                            
                            if (h->nav) {
                                strcpy(line, h->nav->command);
                                pos = strlen(line);
                                printf("%s", line);
                            } else {
                                line[0] = '\0';
                                pos = 0;
                            }
                            fflush(stdout);
                        }
                        break;
                        
                    case 'B': // Down arrow
                        if (h && h->history_cnt > 0) {
                            if (h->nav && h->nav->next) {
                                h->nav = h->nav->next;
                            } else {
                                h->nav = NULL;
                            }
                            
                            // Clear current line and show history
                            printf("\r%s", prompt);
                            for (int i = 0; i < pos; i++) printf(" ");
                            printf("\r%s", prompt);
                            
                            if (h->nav) {
                                strcpy(line, h->nav->command);
                                pos = strlen(line);
                                printf("%s", line);
                            } else {
                                line[0] = '\0';
                                pos = 0;
                            }
                            fflush(stdout);
                        }
                        break;
                        
                    case 'C': // Right arrow
                        if (pos < (int)strlen(line)) {
                            pos++;
                            printf("\033[C"); // Move cursor right
                        }
                        break;
                        
                    case 'D': // Left arrow
                        if (pos > 0) {
                            pos--;
                            printf("\033[D"); // Move cursor left
                        }
                        break;
                }
            }
        } else if (ch == 127 || ch == 8) { // Backspace
            if (pos > 0) {
                // Move cursor back, print space, move cursor back
                printf("\b \b");
                memmove(&line[pos-1], &line[pos], strlen(line) - pos + 1);
                pos--;
            }
        } else if (ch >= 32 && ch <= 126) { // Printable characters
            // Insert character at cursor position
            memmove(&line[pos+1], &line[pos], strlen(line) - pos + 1);
            line[pos] = ch;
            pos++;
            
            // Print the character and any characters after it
            printf("%c", ch);
            if (pos < (int)strlen(line)) {
                printf("%s", &line[pos]);
                // Move cursor back to correct position
                for (int i = strlen(line); i > pos; i--) {
                    printf("\b");
                }
            }
        }
    }
    
    printf("\n");
    fflush(stdout);
    
    // Restore terminal to original state immediately after input
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    
    // Reset navigation cursor
    if (h) h->nav = NULL;
    
    return strdup(line);
}
