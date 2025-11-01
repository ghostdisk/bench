project(bench)

add_library(bench_core STATIC
	bench/bench.cpp)

target_include_directories(bench_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")