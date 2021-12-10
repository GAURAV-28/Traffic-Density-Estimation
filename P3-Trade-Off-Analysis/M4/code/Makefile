CC=g++
CFLAGS= -c -Wall
LIB= `pkg-config --cflags --libs opencv`

all:
	$(CC) m4_4.cpp -o output -pthread -std=c++11 $(LIB) 

clean:
	rm output
