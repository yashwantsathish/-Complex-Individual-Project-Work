CC = gcc

CFLAGS = -Wall -g

LD = gcc

LDFLAGS =

all: hencode hdecode

hencode: hencode.o htable.o 
	$(CC) $(CFLAGS) -o hencode hencode.o htable.o

hdecode: hdecode.o htable.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o htable.o

htable.o: htable.c
	$(CC) $(CFLAGS) -c -o htable.o htable.c

hencode.o: hencode.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c

hdecode.o: hdecode.c
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c

clean:
	rm -f *.o
