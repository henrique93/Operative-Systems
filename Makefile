# Makefile for par-shell, version 1
# Sistemas Operativos, DEI/IST/ULisboa 2015-16

CFLAGS=-g -Wall -pedantic

all: par-shell par-shell-terminal fibonacci

par-shell: par-shell.o commandlinereader.o list.o Integer_List.o
	gcc -pthread -o par-shell par-shell.o commandlinereader.o list.o Integer_List.o

par-shell.o: par-shell.c
	gcc $(CFLAGS) -c par-shell.c

par-shell-terminal: par-shell-terminal.o
	gcc -pthread -o par-shell-terminal par-shell-terminal.o

par-shell-terminal.o: par-shell-terminal.c
	gcc $(CFLAGS) -c par-shell-terminal.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c

list.o: list.c list.h
	gcc $(CFLAGS) -c list.c

Integer_List.o: Integer_List.c Integer_List.h
	gcc $(CFLAGS) -c Integer_List.c

fibonacci:
	gcc $(CFLAGS) -o fibonacci fibonacci.c

clean:
	rm *.o par-shell par-shell-terminal fibonacci
