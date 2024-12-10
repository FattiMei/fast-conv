#include <iostream>
#include "automata.hpp"


#define NCOLS 181
#define NROWS 80


void setup_line(const int n, Cell line[]) {
	for (int i = 0; i < n; ++i) {
		line[i] = (Cell) 0;
	}

	line[n/2] = (Cell) 1;
}


void print_line_ascii(const int n, const Cell line[]) {
	for (int i = 0; i < n; ++i) {
		// strange, if I remove the () it prints '0' and '1'
		std::cout << (line[i] ? 'x' : 'o');
	}

	std::cout << std::endl;
}


int main(int argc, char *argv[]) {
	int rule = 0;

	if (argc == 2) {
		rule = std::atoi(argv[1]);
	}
	else {
		std::cerr << "Usage: wolf <rule number>" << std::endl;
		return 1;
	}

	Cell line[NCOLS];
	Cell temp[NCOLS];

	setup_line(NCOLS, line);
	print_line_ascii(NCOLS, line);

	for (int i = 1; i < NROWS; ++i) {
		compute_new_line_reference(NCOLS, line, temp, rule);
		std::swap(line, temp);

		print_line_ascii(NCOLS, line);
	}

	return 0;
}
