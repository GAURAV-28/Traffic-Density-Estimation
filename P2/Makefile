CC=g++
CFLAGS= -c -Wall
LIB= `pkg-config --cflags --libs opencv`

all:
	$(CC) tem.cpp -o output -pthread -std=c++11 $(LIB)

clean:
	rm output
