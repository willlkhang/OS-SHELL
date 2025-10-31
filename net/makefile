all: ser cli

ser: ser.o authen.o telnet.o
	gcc -Wall ser.o authen.o telnet.o -o ser
    
cli: cli.o telnet.o authen.o shell_communicate.o
	gcc -Wall cli.o telnet.o authen.o shell_communicate.o -o cli

ser.o: ser.c
	gcc -Wall -c ser.c 

cli.o: cli.c
	gcc -Wall -c cli.c

telnet.o: telnet.c telnet.h
	gcc -Wall -c telnet.c telnet.h

shell_communicate.o: shell_communicate.c shell_communicate.h
	gcc -Wall -c shell_communicate.c shell_communicate.h

authen.o: authen.c authen.h account_db.h
	gcc -Wall -c authen.c authen.h account_db.h

cls: 
	rm -f *.gch *.o
clean:
	rm -f *.gch *.o cli ser