project(bench)

add_library(bench_core STATIC
	bench/application.cpp
	bench/coroutine.cpp
	bench/win32.cpp
	bench/renderer.cpp
	bench/time.cpp
	bench/ini.cpp
	bench/file.cpp
)

target_include_directories(bench_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_property(TARGET bench_core PROPERTY CXX_STANDARD 14)

if (WIN32)
	target_compile_definitions(bench_core PUBLIC BENCH_WIN32)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(bench_core PUBLIC BENCH_DEBUG)
endif()