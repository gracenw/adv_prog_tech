# Author: Gracen Wallace
# Class: ECE 6122 A
# Last Date Modified: 11/06/21
# 
# Description:
# Simple Makefile for SFML TCP client/server application.

CC = g++

all: client server

%.o: %.cpp
	$(CC) -c -o $@ $<

server: server.cpp
	$(CC) $^ -lsfml-network -lsfml-system -o server.out

client: client.cpp
	$(CC) $^ -lsfml-network -lsfml-system -o client.out

clean:
	rm -f *.out *.log