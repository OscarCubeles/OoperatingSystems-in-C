all: Objects Atreides Fremen Harkonen

Objects:
	gcc -Wall -Wextra -c Fremen.c -o Fremen.o
	gcc -Wall -Wextra -c Harkonen.c -o Harkonen.o
	gcc -Wall -Wextra -c Atreides.c -o Atreides.o
	gcc -Wall -Wextra -c command.c -o command.o
	gcc -Wall -Wextra -c connection.c -o connection.o
	gcc -Wall -Wextra -c readfile.c -o readfile.o
	gcc -Wall -Wextra -c mystring.c -o mystring.o
	gcc -Wall -Wextra -c myprint.c -o myprint.o
	gcc -Wall -Wextra -c user.c -o user.o

Fremen: command.o mystring.o readfile.o connection.o myprint.o user.o
	gcc Fremen.o command.o mystring.o readfile.o connection.o myprint.o user.o -o Fremen -Wall -Wextra

Atreides: Atreides.o command.o mystring.o readfile.o connection.o myprint.o user.o
	gcc Atreides.o command.o mystring.o readfile.o connection.o myprint.o user.o -o Atreides -Wall -Wextra -lpthread

Harkonen: Harkonen.o command.o mystring.o readfile.o connection.o myprint.o user.o
	gcc Harkonen.o command.o mystring.o readfile.o connection.o myprint.o user.o -o Harkonen -Wall -Wextra

clean:
	rm -f *.o Fremen Atreides Harkonen
