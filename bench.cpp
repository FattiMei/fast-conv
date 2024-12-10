#include <vector>
#include <cstdlib>
#include <benchmark/benchmark.h>
#include "automata.hpp"


template <void (*fcn)(const int n, const Cell[], Cell [], const int)>
static void BM_automata_step(benchmark::State &state) {
	const int n = state.range(0) + 1;
	const int rule = std::rand() % 256;

	std::vector<Cell> line(n);
	std::vector<Cell> temp(n);

	for (auto &x : line) {
		x = std::rand() % 2;
	}

	for (auto _ : state) {
		fcn(n, line.data(), temp.data(), rule);
		line.swap(temp);
	}
}

BENCHMARK_TEMPLATE(BM_automata_step, compute_new_line_corner_cases   )->RangeMultiplier(2)->Range(64, 1<<20);
BENCHMARK_TEMPLATE(BM_automata_step, compute_new_line_reuse          )->RangeMultiplier(2)->Range(64, 1<<20);
BENCHMARK_TEMPLATE(BM_automata_step, compute_new_line_full_reuse     )->RangeMultiplier(2)->Range(64, 1<<20);
BENCHMARK_TEMPLATE(BM_automata_step, compute_new_line_packed<int32_t>)->RangeMultiplier(2)->Range(64, 1<<20);
BENCHMARK_TEMPLATE(BM_automata_step, compute_new_line_packed<int64_t>)->RangeMultiplier(2)->Range(64, 1<<20);

#if COMPILER_SUPPORTS_EXPERIMENTAL_SIMD
BENCHMARK_TEMPLATE(BM_automata_step, compute_new_line_simd           )->RangeMultiplier(2)->Range(64, 1<<20);
#endif

BENCHMARK_MAIN();
