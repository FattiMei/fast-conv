#include <array>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include "automata.hpp"


constexpr int size = 16;
constexpr int rule = 103;
using Line = std::array<Cell, size>;


int main() {
	Line start;
	Line reference;
	Line alternative;

	for (int i = 0; i < size; ++i) {
		start[i] = std::rand() % 2;
	}

	compute_new_line_reference(size, start.data(), reference.data(), rule);

	compute_new_line_branchless(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	compute_new_line_corner_cases(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	compute_new_line_reuse(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	compute_new_line_full_reuse(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	compute_new_line_simd<2>(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	compute_new_line_simd<4>(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	compute_new_line_simd<8>(size, start.data(), alternative.data(), rule);
	assert(reference == alternative);

	return 0;
}
