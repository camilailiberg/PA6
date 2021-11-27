CC=gcc 
CFLAGS=-I. -g -pthread -w
DEPS = randomgen.h
OBJ = randomgen.o wordpuzzle.o yourprogram.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
