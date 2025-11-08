#pragma once

#include <assert.h>
#include <sal.h>

namespace bench {

typedef signed char        I8;
typedef short              I16;
typedef int                I32;
typedef long long          I64;
typedef unsigned char      U8;
typedef unsigned short     U16;
typedef unsigned int       U32;
typedef unsigned long long U64;

_When_(!condition, _Analysis_noreturn_)
void AssertAlways(bool condition, const char* fail_message);

template <typename T>
struct RemoveReference { using Type = T; };

template <typename T>
struct RemoveReference<T&> { using Type = T; };

template <typename T>
struct RemoveReference<T&&> { using Type = T; };

template <typename T>
constexpr typename RemoveReference<T>::Type&& Move(T&& value) noexcept {
    return static_cast<typename RemoveReference<T>::Type&&>(value);
}

}


// Forward declare some commonly used STL/libc symbols. The corresponding headers (<new>, <stdlib.h>, etc) can be
// quite heavy and increase compile times, so we declare only what we need.
#ifdef BENCH_WIN32

inline void* __CRTDECL operator new(size_t _Size, _Writable_bytes_(_Size) void* _Where) noexcept;
#include <corecrt_malloc.h>

#else
#	include <new>
#endif


#ifndef BENCH_WIN32
#	error BENCH_WIN32 must be defined
#endif
