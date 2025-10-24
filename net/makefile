all: ser cli

ser: se.o stream.o
	gcc se.o stream.o -o ser

cli: cl.o stream.o
	gcc cl.o stream.o -o cli

se.o: se.c
	gcc -c se.c

cl.o: cl.c
	gcc -c cl.c

stream.o: stream.c stream.h
	gcc -c stream.c stream.h

clean:
	rm -f *.gch *.o cli ser