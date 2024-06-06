SOURCES=$(wildcard ./src/*.c)
HEADERS=$(SOURCES:.c=.h)
#FLAGS=-DDEBUG -g
FLAGS=-g

all: main

main:
	mpicc $(SOURCES) $(FLAGS) -o ./build/main

clear: clean

clean:
	rm main a.out

run: main Makefile tags
	mpirun -oversubscribe -np 8 ./main
