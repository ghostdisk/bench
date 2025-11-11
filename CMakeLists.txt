cmake_minimum_required(VERSION 3.24)
project(bench)

enable_language(ASM_MASM)

add_library(bench_core STATIC
	bench/core/arena.cpp
	bench/core/file.cpp
	bench/core/format.cpp
	bench/core/string.cpp
	bench/core/time.cpp
	bench/application.cpp
	bench/asset.cpp
	bench/coroutine.cpp
	bench/ini.cpp
	bench/renderer.cpp
	bench/sys_win32.cpp
	bench/coroutine_win32_x86.asm
	vendor/fast_obj.c
	vendor/murmurhash.c)


target_include_directories(bench_core PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor")

target_compile_definitions(bench_core PUBLIC
	BENCH_WIN32
	BENCH_DEBUG)

set_property(TARGET bench_core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

