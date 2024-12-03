#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include "automata.hpp"


#define TEST_IMPLEMENTATION(foo) assert(test_suite(foo, #foo))


constexpr int size = 100;
constexpr int rule = 103;


Cell start[size];
Cell reference[size];
Cell alternative[size];


bool single_test_case(
	void (*impl) (const int, const Cell[], Cell[], const int),
	Cell start[],
	Cell reference[],
	Cell alternative[],
	const int n,
	const int rule = 102
) {
	for (int i = 0; i < n; ++i) {
		start[i] = std::rand() % 2;
	}

	compute_new_line_reference(n, start, reference, rule);
	impl(n, start, alternative, rule);

	for (int i = 0; i < n; ++i) {
		if (reference[i] != alternative[i]) {
			return false;
		}
	}

	return true;
}


bool test_suite(
	void (*impl) (const int, const Cell[], Cell[], const int),
	const std::string &name
) {
	bool has_failed_some_test = false;

	for (int n = 3; n < size; ++n) {
		for (int i = 0; i < n; ++i) {
			alternative[i] = -1;
		}

		if (false == single_test_case(impl, start, reference, alternative, n)) {
			has_failed_some_test = true;
			std::cerr << "ERROR (" << name << "): failed test case when n = " << n << std::endl;
		}
	}

	return not has_failed_some_test;
}


int main() {
	TEST_IMPLEMENTATION(compute_new_line_reference);
	TEST_IMPLEMENTATION(compute_new_line_branchless);
	TEST_IMPLEMENTATION(compute_new_line_corner_cases);
	TEST_IMPLEMENTATION(compute_new_line_reuse);
	TEST_IMPLEMENTATION(compute_new_line_full_reuse);
	TEST_IMPLEMENTATION(compute_new_line_packed<int32_t>);
	TEST_IMPLEMENTATION(compute_new_line_packed<int64_t>);

	return 0;
}
