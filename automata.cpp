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

	for (i = 0; i < n-3; i += 3) {
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


template <typename Integer>
void compute_new_line_packed(const int n, const Cell above[], Cell below[], const int rule) {
	constexpr int SIZEOF = sizeof(Integer);
	constexpr int LANE_WIDTH = 8 * SIZEOF;
	Integer vector, acc, left = 0, right;
	int i, number;

	for (i = 0; i+SIZEOF < n; i += SIZEOF) {
		vector = *((Integer *) (above + i));
		right = above[i+SIZEOF];

		acc =
			  (((vector << 8) | left) << 2)
			+ (vector << 1)
			+ ((vector >> 8) | (right << (LANE_WIDTH - 8)));

		for (int j = 0; j < SIZEOF; ++j) {
			number = acc & 0xFF;
			below[i+j] = (rule >> number) % 2;

			acc = acc >> 8;
		}

		left = vector >> (LANE_WIDTH - 8);
	}

	// at this point we are on a cell with a sure left neighbour
	// we simply copy an existing implementation
	Cell local[3] = {static_cast<Cell>(left), above[i], 0};

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


template void compute_new_line_packed<int32_t>(const int, const Cell[], Cell[], const int);
template void compute_new_line_packed<int64_t>(const int, const Cell[], Cell[], const int);
