#ifndef COMMAND_H
#define COMMAND_H

#define MAX_COMMANDS 100

typedef struct Command_struct
{   
    //parser helper
    int first; // index of first token
    int last; //index of last token
    char *sep; //separators

    //execution
    char *pathname;
    int argc;
    char **argv;
    char *stdin_file;   // if not NULL, points to the file name for stdin redirection
    char *stdout_file;  // if not NULL, points to the file name for stdout redirection    
    char suffix; // ' ', '&', ';', '|' 
} 
Command;

typedef struct Command_line_Strucr
{
    Command commands[MAX_COMMANDS];
    int amount; // number of commands from the input
    int background; // for &
}
CommandLine;

#endif