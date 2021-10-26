CC = g++

%.o: %.cpp
	$(CC) -c -o $@ $<

one: Lab2_Problem1.cpp
	$(CC) -lpthread -O3 -o one.out $^

two: Lab2_Problem2.cpp
	$(CC) -fopenmp -O3 -o two.out $^

clean:
	rm -f *.out *.txt