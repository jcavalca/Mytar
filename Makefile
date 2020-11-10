CC = gcc

CFLAGS = -Wall -pedantic -g


all: mytar
mytar: mytar.c check.c format.c oct.c print.c special.c tree.c write.c list.c extract.c share.c define.h
	$(CC) $(CFLAGS) -o mytar mytar.c

