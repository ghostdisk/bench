#pragma once

#include <stdint.h>
#include <assert.h>
#include <sal.h>

/*
 * Bench SDK reserves the right to pollute your global namespace with our
 * preferred naming for fixed-size primitive types. If another SDK pollutes your
 * global namespace with conflicting definitions for these symbols, please
 * contact the representatives of that other SDK, as they most likely haven't
 * reserved the right to pollute your global namespace.
 */
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   I8;
typedef int16_t  I16;
typedef int32_t  I32;
typedef int64_t  I64;
typedef float    F32;
typedef double   F64;
typedef bool     Bool;

namespace bench {

_When_(!condition, _Analysis_noreturn_)
void AssertAlways(bool condition, const char* fail_message);

}

#ifndef BENCH_WIN32
#	error BENCH_WIN32 must be defined
#endif