# A Makefile to build 'myshell' project

myshell : parser.o myshell.o globals.o execute.o
	cc -std=c99 -Wall -pedantic -Werror -o myshell \
                  parser.o globals.o myshell.o execute.o -lm
parser.o : parser.c myshell.h
	cc -std=c99 -Wall -pedantic -Werror -c parser.c
myshell.o : myshell.c myshell.h
	cc -std=c99 -Wall -pedantic -Werror -c myshell.c
globals.o : globals.c myshell.h
	cc -std=c99 -Wall -pedantic -Werror -c globals.c
execute.o : execute.c myshell.h
	cc -std=c99 -Wall -pedantic -Werror -c execute.c
clean:
	rm -f myshell parser.o myshell.o globals.o execute.o
