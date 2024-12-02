CXX      = g++
WARNINGS = -Wall -Wextra -Wpedantic
OPT      = -O2


all: test wolf


test: regression
	./$^


wolf: build/main.o build/automata.o
	$(CXX) -o $@ $^


regression: build/regression.o build/automata.o
	$(CXX) -o $@ $^


build/%.o: %.cpp
	$(CXX) -c -o $@ $^ $(WARNINGS) $(OPT)


build/automata.s: automata.cpp
	$(CXX) -S -o $@ $^ $(WARNINGS) $(OPT)


.PHONY: clean
clean:
	rm -rf build/*.o build/*.s
