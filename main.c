#include "command.h"
#include "signal_processor.h"
#include "utilities.h"
#include "parser.h"
#include "builtins_executor.h"
#include "command_executor.h"
#include "history_process.h"
#include "history.h"
#include "interactive_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    activate_signal_handlers();
    init_interactive_input();

    char *line = NULL, prompt_buf[128];
    strcpy(prompt_buf, "% ");

    history *h = malloc(sizeof(history));
    if(h == NULL) perror("Failed to allocate history"), exit(1);
    setup_history(h);

    while (1) {
        // Use interactive input with arrow key support
        line = get_interactive_input(prompt_buf, h);
        if (!line) break;
        
        // Skip empty lines
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        //get_line_with_history(line, sizeof(line), h);
        //printf("\n"); // Move to the next line after command entry

        // Check for history expansion before adding to history
        char *expanded_line = expand_history_command(line, h);
        if (strcmp(expanded_line, line) != 0) {
            printf("%s\n", expanded_line);  // Show the expanded command
            free(line);
            line = expanded_line;
        } else {
            free(expanded_line);
        }

        history_add(h, line);

        //parse user's input to struct Commandline
        CommandLine cl;
        int ncmd = parse_line(line, &cl);

        if(!parser_erorr(ncmd)) continue; // if there is error, rise error, and skip that error

        //execute commands in cl
        execute_commads(&ncmd, &cl, prompt_buf, h);

        //free the parsed command from user's input string
        free_parsed_commands(cl.commands, ncmd);
    }

    history_free(h);
    free(line);
    cleanup_interactive_input();
    return 0;
}