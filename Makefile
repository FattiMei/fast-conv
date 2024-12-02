CXX      = g++
PYTHON   = python3
WARNINGS = -Wall -Wextra -Wpedantic
OPT      = -O2


all: test wolf


test: regression
	./$^


bench: report.json
	$(PYTHON) plot.py report.json


wolf: build/main.o build/automata.o
	$(CXX) -o $@ $^


regression: build/regression.o build/automata.o
	$(CXX) -o $@ $^


benchmark: build/bench.o build/automata.o
	$(CXX) -o $@ $^ -lpthread -lbenchmark


build/%.o: %.cpp
	$(CXX) -c -o $@ $^ $(WARNINGS) $(OPT)


build/automata.s: automata.cpp
	$(CXX) -S -o $@ $^ $(WARNINGS) $(OPT)


report.json: benchmark
	./$^ --benchmark_out=report.json --benchmark_out_format=json


.PHONY: clean
clean:
	rm -rf build/*.o build/*.s report.json
