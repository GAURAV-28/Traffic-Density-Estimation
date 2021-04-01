CC=g++
CFLAGS= -c -Wall
LIB= `pkg-config --cflags --libs opencv`

all:
	$(CC) m1.cpp -o m1_output -pthread -std=c++11 $(LIB) 
	$(CC) m2.cpp -o m2_output -pthread -std=c++11 $(LIB) 
	$(CC) m3m.cpp -o m3_output -pthread -std=c++11 $(LIB) 
	$(CC) m4_4.cpp -o m4_output -pthread -std=c++11 $(LIB)

method1:
	$(CC) m1.cpp -o m1_output -pthread -std=c++11 $(LIB) 

method2:
	$(CC) m2.cpp -o m2_output -pthread -std=c++11 $(LIB) 

method3:
	$(CC) m3m.cpp -o m3_output -pthread -std=c++11 $(LIB) 

method4:
	$(CC) m4_4.cpp -o m4_output -pthread -std=c++11 $(LIB)

clean:
	rm m1_output m2_output m3_output m4_output
