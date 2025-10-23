# OS-SHELL - Unix Shell Implementation

## Overview
A complete Unix shell implementation with all required features for the assignment.

## Core Files
- `main.c` - Main shell program
- `makefile` - Build configuration
- `command.h` - Command structure definitions

## Parser & Tokenizer
- `parser.c/h` - Command line parsing
- `tokenizer.c/h` - Tokenization with quotes and escapes
- `tokenizer_utilities.c/h` - Tokenizer helper functions
- `separators.c/h` - Separator handling

## Execution
- `command_executor.c/h` - Command execution
- `builtins_executor.c/h` - Built-in commands (prompt, pwd, cd, history, exit)
- `utilities.c/h` - Utility functions and redirections
- `wildcard_handler.c/h` - Wildcard expansion using glob()

## History & Interactive Features
- `history_process.c/h` - Command history with expansion (!x commands)
- `interactive_input.c/h` - Arrow key navigation with ncurses
- `history.h` - History data structures

## Signal Handling
- `signal_processor.c/h` - Signal handling (CTRL-C, CTRL-Z, CTRL-\)

## Build Instructions
```bash
make clean
make
./run
```

## Features Implemented
- ✅ All built-in commands (prompt, pwd, cd, history, exit)
- ✅ I/O redirection (<, >, 2>)
- ✅ Pipeline support (|)
- ✅ Background execution (&)
- ✅ Sequential execution (;)
- ✅ Wildcard expansion (*, ?)
- ✅ Command history with arrow keys
- ✅ History expansion (!x commands)
- ✅ Signal handling
- ✅ Zombie process prevention
