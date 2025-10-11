// #include "history_up_down_key.h"

// #include <stdlib.h>
// #include <stdio.h> //for EOF
// #include <string.h>

// char *history_key_up(history *h){
//     if(h->history_idx > 0){
//         h->history_idx--;
//         return h->history_log[h->history_idx];
//     }
//     return NULL;
// }

// char *history_key_down(history * h){
//     if(h->history_idx < h->history_cnt){
//         h->history_idx++;
//         if(h->history_idx == h->history_cnt){
//             return "";
//         }
//         return h->history_log[h->history_idx];
//     }
//     return NULL;
// }

// int get_line_with_history(char *line, int size, history *h){
//     int i = 0;
//     char *current_cmd = NULL;

//     while (i < size - 1) {
//         int c = getchar();
//         if (c == EOF || c == '\n') {
//             line[i] = '\0';
//             return i;
//         } else if (c == '\x1b') { // ANSI Escape Sequence for arrow keys
//             getchar(); // Skip the '['
//             switch (getchar()) {
//                 case 'A': // Up Arrow
//                     current_cmd = history_key_up(h);
//                     if (current_cmd) {
//                         // Clear the line, copy history, and move cursor
//                         printf("\r\033[K%s %s", "osh>", current_cmd);
//                         strcpy(line, current_cmd);
//                         i = strlen(line);
//                     }
//                     break;
//                 case 'B': // Down Arrow
//                     current_cmd = history_key_down(h);
//                     if (current_cmd) {
//                         printf("\r\033[K%s %s", "osh>", current_cmd);
//                         strcpy(line, current_cmd);
//                         i = strlen(line);
//                     }
//                     break;
//             }
//         } else if (c == 127 || c == 8) { // Handle backspace
//              if (i > 0) {
//                 i--;
//                 printf("\b \b"); // Move cursor back, write space, move back again
//             }
//         } else {
//             line[i++] = c;
//             putchar(c); // Echo character to the screen
//         }
//     }
//     line[i] = '\0';
//     return i;
// }