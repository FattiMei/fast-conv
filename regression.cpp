#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include "automata.hpp"


#define TEST_IMPLEMENTATION(foo) assert(test_implementation(foo))


constexpr int size = 100;
constexpr int rule = 103;
using Line = std::vector<Cell>;


bool test_implementation(void (*impl) (const int, const Cell[], Cell[], const int)) {
	Line start(size);
	Line reference(size);
	Line alternative(size);

	for (int n = 3; n < size; ++n) {
		const int rule = std::rand() % 256;

		for (int i = 0; i < size; ++i) {
			start[i] = std::rand() % 2;
		}

		compute_new_line_reference(n, start.data(), reference.data(), rule);
		impl(n, start.data(), alternative.data(), rule);

		for (int i = 0; i < size; ++i) {
			if (reference[i] != alternative[i]) {
				std::cerr << "Found error when size = " << n << std::endl;
				return false;
			}
		}
	}

	return true;
}


int main() {
	TEST_IMPLEMENTATION(compute_new_line_reference);
	TEST_IMPLEMENTATION(compute_new_line_branchless);
	TEST_IMPLEMENTATION(compute_new_line_corner_cases);
	TEST_IMPLEMENTATION(compute_new_line_reuse);
	TEST_IMPLEMENTATION(compute_new_line_full_reuse);

	return 0;
}
