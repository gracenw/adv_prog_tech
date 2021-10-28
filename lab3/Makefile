CC = g++

%.o: %.cpp
	$(CC) -c -o $@ $<

all: main.cpp
	$(CC) $^ -lsfml-graphics -lsfml-window -lsfml-system -o main.exe

clean:
	rm -f *.out