CXX = g++
CXXFLAGS = -g -std=c++11 -Wall
OBJECTS = main.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	./main

run:
	./main

clean:
	rm *.o *.out main main.exe test test.exe glitched.png
