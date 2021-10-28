# Author: Gracen Wallace
# Class: ECE 6122 A
# Last Date Modified: 10/28/21
# 
# Description:
# Simple makefile for Buzzy's revenge; Just run 'make && ./main'

CC = g++

%.o: %.cpp
	$(CC) -c -o $@ $<

all: main.cpp
	$(CC) $^ -lsfml-graphics -lsfml-window -lsfml-system -o main

clean:
	rm -f *.out