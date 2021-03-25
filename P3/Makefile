CC=g++
CFLAGS= -c -Wall
LIB= `pkg-config --cflags --libs opencv`

all:
	$(CC) m4.cpp -o output -pthread -std=c++11 $(LIB) -lpthread

clean:
	rm output
