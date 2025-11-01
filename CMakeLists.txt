project(bench)

add_library(bench_core STATIC
	bench/bench.cpp
	bench/coroutine.cpp
	bench/bench_win32.cpp
	bench/file.cpp
)

target_include_directories(bench_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if (WIN32)
	target_compile_definitions(bench_core PUBLIC BENCH_WIN32)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(bench_core PUBLIC BENCH_DEBUG)
endif()