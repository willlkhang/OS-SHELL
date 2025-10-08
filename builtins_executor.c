#include "builtins_executor.h"
#include "history_process.h"

#include <string.h> //prefix str*
#include <stdio.h> // print
#include <unistd.h> //getcwd
#include <limits.h> //PATH_MAX
#include <stdlib.h>

int is_builtins(char *cmd)
{
    return (
        (strcmp(cmd, "prompt") == 0) || 
        (strcmp(cmd, "pwd") == 0) || 
        (strcmp(cmd, "cd") == 0) || 
        (strcmp(cmd, "history") == 0) ||
        (strcasecmp(cmd, "exit") == 0)
    );
}


int builtin_handler(Command *cmd, char prompt_buf[], size_t prompt_buf_len) {

    if (strcmp(cmd->argv[0], "exit") == 0 || strcmp(cmd->argv[0], "bye") == 0) {
        printf("\n\nYou terminated the SHELL\n\n");
        exit(0);
    }

    if (cmd->argc == 0) 
        return 0;

    if (strcmp(cmd->argv[0], "cd") == 0) {
        const char *target = (cmd->argc > 1) ? cmd->argv[1] : getenv("HOME");
        if (!target) target = "/";
        if (chdir(target) != 0) perror("cd");
        return 1;
    }

    if (strcmp(cmd->argv[0], "pwd") == 0) {
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    }

    if (strcmp(cmd->argv[0], "prompt") == 0) {
        if (cmd->argc > 1) {
            // copy new prompt
            snprintf(prompt_buf, prompt_buf_len, "%s ", cmd->argv[1]);
        } else {
            //reset
            snprintf(prompt_buf, prompt_buf_len, "%% ");
        }
        return 1;
    }

    /* NOTE: history, !! and !n should be handled BEFORE parse_line or
       you can implement history lookup here (requires history store). */

    return 0; /* not a builtin we handle here */
}