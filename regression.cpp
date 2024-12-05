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
	const int n,
	Cell start[],
	Cell reference[],
	Cell alternative[],
	const int rule
) {
	for (int i = 0; i < n; ++i) {
		start[i] = std::rand() % 2;
	}

	// I want to be really sure that other test cases don't influence eachother
	for (int i = 0; i < n; ++i) {
		alternative[i] = 9;
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


void log_test_outcome(const std::string &name, const int n, const Cell start[], const Cell reference[], const Cell alternative[], const int rule) {
	std::cerr << "On rule " << rule << "`" << name << "` has produced" << std::endl;

	std::cerr << "starting   : ";
	for (int i = 0; i < n; ++i) std::cerr << static_cast<int>(start[i]);
	std::cerr << std::endl;

	std::cerr << "reference  : ";
	for (int i = 0; i < n; ++i) std::cerr << static_cast<int>(reference[i]);
	std::cerr << std::endl;

	std::cerr << "alternative: ";
	for (int i = 0; i < n; ++i) std::cerr << static_cast<int>(alternative[i]);
	std::cerr << std::endl;
}


bool test_suite(
	void (*impl) (const int, const Cell[], Cell[], const int),
	const std::string &name
) {
	bool has_failed_some_test = false;

	for (int n = 3; n < size; ++n) {
		const int rule = std::rand() % 256;

		if (false == single_test_case(impl, n, start, reference, alternative, rule)) {
			has_failed_some_test = true;

			log_test_outcome(name, n, start, reference, alternative, rule);
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
	TEST_IMPLEMENTATION(compute_new_line_simd);

	return 0;
}
