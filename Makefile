CC = gcc

CFLAGS =  -Wall -pedantic -g


all: mytar.c
	make mytar
mytar: mytar.c
	$(CC) $(CFLAGS) -o mytar mytar.c
clean: 
	rm mytar

