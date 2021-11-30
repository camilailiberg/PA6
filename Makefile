CC = gcc
CFLAGS = -I. -g -pthread -w

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

wordpuzzle: wordpuzzle.o
	$(CC) -o $@ $^ $(CFLAGS)

	