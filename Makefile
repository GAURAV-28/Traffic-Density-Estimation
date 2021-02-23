CC=g++
CFLAGS= -c -Wall
LIB= `pkg-config --cflags --libs opencv`

all:
	$(CC) a1p1.cpp -o output -pthread -std=c++11 $(LIB)
