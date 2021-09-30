CC = g++
LFLAGS = -pthread

%.o: %.cpp
	$(CC) -c -o $@ $<

one: Lab2_Problem1.cpp
	$(CC) $(LFLAGS) -o one.out $^

two: Lab2_Problem2.cpp
	$(CC) $(LFLAGS) -o two.out $^

clean:
	rm -f *.out