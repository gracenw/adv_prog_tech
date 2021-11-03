# Author: Gracen Wallace
# Class: ECE 6122 A
# Last Date Modified: 
# 
# Description:
# 

CC = g++

all: client server

%.o: %.cpp
	$(CC) -c -o $@ $<

server: server.cpp
	$(CC) $^ -lsfml-network -o server.out

client: client.cpp
	$(CC) $^ -lsfml-network -o client.out

clean:
	rm -f *.out