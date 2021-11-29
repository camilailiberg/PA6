CC=gcc 
CFLAGS=-I. -g -w -lpthread 
DEPS = randomgen.h
OBJ = randomgen.o wordpuzzle.o yourprogram.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

randomgen: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# gcc experimentingWithThreads.c -o experimentingWithThreads -lpthread