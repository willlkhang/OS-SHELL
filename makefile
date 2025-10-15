run: main.o signal_processor.o parser.o utilities.o separators.o tokenizer.o builtins_executor.o command_executor.o tokenizer_utilities.o command_setup.o history_process.o
	gcc main.o signal_processor.o parser.o utilities.o separators.o tokenizer.o builtins_executor.o command_executor.o tokenizer_utilities.o command_setup.o history_process.o -o run

main.o: main.c signal_processor.h command.h parser.h utilities.h separators.h tokenizer.h builtins_executor.h command_executor.h tokenizer_utilities.h command_setup.h history.h history_process.h
	gcc -c main.c

signal_processor.o: signal_processor.c signal_processor.h
	gcc -c signal_processor.c signal_processor.h

parser.o: parser.c parser.h
	gcc -c parser.c parser.h

utilities.o: utilities.c utilities.h
	gcc -c utilities.c utilities.h

separators.o: separators.c separators.h
	gcc -c separators.c separators.h

tokenizer.o: tokenizer.c tokenizer.h
	gcc -c tokenizer.c tokenizer.h

builtins_executor.o: builtins_executor.c builtins_executor.h
	gcc -c builtins_executor.c builtins_executor.h

command_executor.o: command_executor.c command_executor.h
	gcc -c command_executor.c command_executor.h

tokenizer_utilities.o: tokenizer_utilities.c tokenizer_utilities.h
	gcc -c tokenizer_utilities.c tokenizer_utilities.h

command_setup.o: command_setup.c command_setup.h
	gcc -c command_setup.c command_setup.h

history_process.o: history_process.c history_process.h
	gcc -c history_process.c history_process.h

clean:
	rm *.o

cleanh:
	rm *.o *.gch