#include <cmath>
#include "automata.hpp"


void compute_new_line_reference(const int n, const Cell above[], Cell below[], const int rule) {
	for (int i = 0; i < n; ++i) {
		int number = 0;

		for (int j = std::max(0, i-1); j <= std::min(n-1, i+1); ++j) {
			if (above[j]) {
				number += 1 << (i - j + 1);
			}
		}

		below[i] = (rule >> number) % 2;
	}
}


void compute_new_line_branchless(const int n, const Cell above[], Cell below[], const int rule) {
	for (int i = 0; i < n; ++i) {
		int number = 0;

		for (int j = std::max(0, i-1); j <= std::min(n-1, i+1); ++j) {
			number += above[j] << (i - j + 1);
		}

		below[i] = (rule >> number) % 2;
	}
}


void compute_new_line_corner_cases(const int n, const Cell above[], Cell below[], const int rule) {
	int number;

	// corner case: left margin
	number = 2*above[0] + 1*above[1];
	below[0] = (rule >> number) % 2;

	// common case: center
	for (int i = 1; i < n-1; ++i) {
		// already resembles a convolution
		number = 4*above[i-1] + 2*above[i] + 1*above[i+1];

		below[i] = (rule >> number) % 2;
	}

	// corner case: right margin
	number = 4*above[n-2] + 2*above[n-1];
	below[n-1] = (rule >> number) % 2;
}


void compute_new_line_reuse(const int n, const Cell above[], Cell below[], const int rule) {
	Cell local[3] = {0, above[0], 0};
	int number;

	for (int i = 0; i < n-1; ++i) {
		local[2] = above[i+1];

		number = 4*local[0] + 2*local[1] + 1*local[2];
		below[i] = (rule >> number) % 2;

		local[0] = local[1];
		local[1] = local[2];
	}

	number = 4*local[0] + 2*local[1];
	below[n-1] = (rule >> number) % 2;
}


void compute_new_line_full_reuse(const int n, const Cell above[], Cell below[], const int rule) {
	Cell local[3] = {0, above[0], 0};
	int i, number;

	for (i = 0; i <= n-3; i += 3) {
		local[2] = above[i+1];
		number = 4*local[0] + 2*local[1] + 1*local[2];
		below[i] = (rule >> number) % 2;

		local[0] = above[i+2];
		number = 4*local[1] + 2*local[2] + 1*local[0];
		below[i+1] = (rule >> number) % 2;

		local[1] = above[i+3];
		number = 4*local[2] + 2*local[0] + 1*local[1];
		below[i+2] = (rule >> number) % 2;
	}

	// rearrange the local array
	switch (i % 3) {
		case 1:
			local[0] = local[1];
			local[1] = local[2];
			break;

		case 2:
			local[1] = local[0];
			local[0] = local[2];
			break;

		default:
			break;
	}

	for (; i < n-1; ++i) {
		local[2] = above[i+1];

		number = 4*local[0] + 2*local[1] + 1*local[2];
		below[i] = (rule >> number) % 2;

		local[0] = local[1];
		local[1] = local[2];
	}

	number = 4*local[0] + 2*local[1];
	below[n-1] = (rule >> number) % 2;
}


void compute_new_line_packed(const int n, const Cell above[], Cell below[], const int rule) {
	int32_t vector, acc, left = 0, right;
	int i, number;

	for (i = 0; i+4 < n; i += 4) {
		vector = *((int32_t *) (above + i));
		right = above[i+4];

		acc =
			  (((vector >> 8) | (left << 24)) << 2)
			+ (vector << 1)
			+ (((vector << 8) | right));

		for (int j = 0; j < 4; ++j) {
			number = (acc >> 24) % 256;
			below[i+j] = (rule >> number) % 2;

			acc = acc << 8;
		}

		left = vector % 256;
	}

	// at this point we are on a cell with a sure left neighbour
	// we simply copy an existing implementation
	Cell local[3] = {left, above[i], 0};

	for (; i < n-1; ++i) {
		local[2] = above[i+1];

		number = 4*local[0] + 2*local[1] + 1*local[2];
		below[i] = (rule >> number) % 2;

		local[0] = local[1];
		local[1] = local[2];
	}

	number = 4*local[0] + 2*local[1];
	below[n-1] = (rule >> number) % 2;
}


template <int UNROLL_FACTOR>
void compute_new_line_simd(const int n, const Cell above[], Cell below[], const int rule) {
	Cell acc[UNROLL_FACTOR];
	Cell shift[UNROLL_FACTOR];
	Cell buffer[UNROLL_FACTOR];
	Cell left = 0, right;

	int i;
	for (i = 0; i+UNROLL_FACTOR < n; i += UNROLL_FACTOR) {
		// all this loops are not fused to help the compiler with the vectorization
		for (int j = 0; j < UNROLL_FACTOR; ++j) {
			acc[j] = 0;
		}

		for (int j = 0; j < UNROLL_FACTOR; ++j) {
			buffer[j] = above[i+j];
		}

		right = above[i+UNROLL_FACTOR];

		for (int j = 0; j < UNROLL_FACTOR; ++j) {
			acc[j] += 2*buffer[j];
		}

		for (int j = UNROLL_FACTOR-1; j > 0; --j) {
			shift[j] = buffer[j-1];
		}
		shift[0] = left;

		for (int j = 0; j < UNROLL_FACTOR; ++j) {
			acc[j] += 4*shift[j];
		}

		for (int j = 0; j < UNROLL_FACTOR-1; ++j) {
			shift[j] = buffer[j+1];
		}
		shift[UNROLL_FACTOR-1] = right;

		for (int j = 0; j < UNROLL_FACTOR; ++j) {
			acc[j] += 1*shift[j];
		}

		for (int j = 0; j < UNROLL_FACTOR; ++j) {
			below[i+j] = (rule >> acc[j]) % 2;
		}

		left = buffer[UNROLL_FACTOR-1];
	}

	// at this point we are on a cell with a sure left neighbour and less than UNROLL_FACTOR cells at its right
	// we simply copy an existing implementation
	Cell local[3] = {left, above[i], 0};
	int number;

	for (; i < n-1; ++i) {
		local[2] = above[i+1];

		number = 4*local[0] + 2*local[1] + 1*local[2];
		below[i] = (rule >> number) % 2;

		local[0] = local[1];
		local[1] = local[2];
	}

	number = 4*local[0] + 2*local[1];
	below[n-1] = (rule >> number) % 2;
}


template void compute_new_line_simd<2>(const int, const Cell[], Cell[], const int);
template void compute_new_line_simd<4>(const int, const Cell[], Cell[], const int);
template void compute_new_line_simd<8>(const int, const Cell[], Cell[], const int);
