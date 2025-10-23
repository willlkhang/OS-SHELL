#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <ncurses.h>

int main(){
    initscr();

    // Print a string (similar to printf) to the virtual screen.
    printw("Hello, ncurses!\n");
    printw("Press any key to exit...");

    // Puts the virtual screen contents onto the actual screen.
    refresh();

    // Wait for user input (a single character).
    getch();

    // End ncurses mode and restore the terminal.
    endwin();

    return 0;

}