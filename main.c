#include "command.h"
#include "signal_processor.h"
#include "utilities.h"
#include "parser.h"
#include "builtins_executor.h"
#include "command_executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    activate_signal_handlers();

    char *line = NULL, prompt_buf[128];
    strcpy(prompt_buf, "% ");

    size_t len = 0;
    ssize_t nread;

    while (1) {
        int flag = prompt_eater(&line, prompt_buf, &len, &nread);
        if(flag == 0) break;
        if(flag == 1) continue;

        //parse user's input to struct Commandline
        CommandLine cl;
        int ncmd = parse_line(line, &cl);

        if(!parser_erorr(ncmd)) continue; // if there is error, rise error, and skip that error
        
        //execute commands in cl
        execute_commads(&ncmd, &cl, prompt_buf);

        //free the parsed command from user's input string
        free_parsed_commands(cl.commands, ncmd);
    }

    free(line);
    return 0;
}