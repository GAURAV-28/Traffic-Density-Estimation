CC=g++
CFLAGS= -c -Wall
LIB= `pkg-config --cflags --libs opencv`

all:
	$(CC) s.cpp -o s -pthread -std=c++11 $(LIB)
