cmake_minimum_required(VERSION 3.10)
project(
	fast-conv
	DESCRIPTION  "From rule 30 automata to vectorized convolutions"
	HOMEPAGE_URL "https://github.com/FattiMei/fast-conv"
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-O3 -march=native -ftree-vectorize)


if (NOT DEFINED COMPILE_SIMD)
	message(STATUS "Checking compiler for experimental simd support")
	try_compile(COMPILE_SIMD ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/simd.cpp)

	if (COMPILE_SIMD)
		add_definitions(-DCOMPILER_SUPPORTS_EXPERIMENTAL_SIMD)
		message(STATUS "Checking compiler for experimental simd support -- ok")
	else()
		message(STATUS "Checking compiler for experimental simd support -- not supported, some features will be ignored")
	endif()
endif()


find_package(Python3 REQUIRED)
find_package(benchmark REQUIRED)


# automata.cpp is compiled into a static library because multiple targets need it and I don't want to recompile it every time
add_library(automata STATIC automata.cpp)


add_executable(wolf main.cpp)
target_link_libraries(wolf PRIVATE automata)


add_executable(regression regression.cpp)
target_link_libraries(regression PRIVATE automata)


if (benchmark_FOUND)
	add_executable(benchmark bench.cpp)
	target_link_libraries(benchmark
		PRIVATE automata
		PRIVATE benchmark::benchmark
	)

	add_custom_command(
		OUTPUT report.json
		DEPENDS benchmark
		COMMAND ./benchmark --benchmark_out=report.json --benchmark_out_format=json
	)

	add_custom_target(
		bench
		DEPENDS report.json
		COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/plot.py report.json
	)
endif ()
