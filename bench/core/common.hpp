#pragma once

#include <assert.h>
#include <sal.h>

#ifndef BENCH_WIN32
#	error BENCH_WIN32 must be defined
#endif

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

// ------------------------------------------------------------
// --- DEFER --------------------------------------------------
// ------------------------------------------------------------

// thank you mr ginger bill
// https://www.gingerbill.org/article/2015/08/19/defer-in-cpp/

template <typename F>
struct privDefer {
	F f;
	privDefer(F f) : f(f) {}
	~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
	return privDefer<F>(f);
}

}

#define BENCH_DEFER_1(x, y) x##y
#define BENCH_DEFER_2(x, y) BENCH_DEFER_1(x, y)
#define BENCH_DEFER_3(x)    BENCH_DEFER_2(x, __COUNTER__)
#define BENCH_DEFER(code)   auto BENCH_DEFER_3(_defer_) = bench::defer_func([&](){code;})

#ifndef BENCH_NO_NONPREFIXED_MACROS
#define DEFER BENCH_DEFER
#endif


// Forward declare some commonly used STL/libc symbols. The corresponding headers (<new>, <stdlib.h>, etc) can be
// quite heavy and increase compile times, so we declare only what we need.
#ifdef BENCH_WIN32

inline void* __CRTDECL operator new(size_t _Size, _Writable_bytes_(_Size) void* _Where) noexcept;
#include <corecrt_malloc.h>

#else
#	include <new>
#endif

// ------------------------------------------------------------
// --- BENCH_ENUM_FLAGS / ENUM_FLAGS --------------------------
// ------------------------------------------------------------

#define BENCH_ENUM_FLAGS(Name, UnderlyingType, ...) \
    struct Name { \
        UnderlyingType value = 0; \
        \
        enum : UnderlyingType { \
            __VA_ARGS__ \
        }; \
        \
        constexpr Name() = default; \
        constexpr Name(UnderlyingType value) : value(value) {} \
        \
        explicit constexpr operator bool() const { return value != 0; } \
        explicit constexpr operator UnderlyingType() const { return value; } \
        \
        constexpr bool operator==(Name other) const { return value == other.value; } \
        constexpr bool operator!=(Name other) const { return value != other.value; } \
    }; \
	\
	constexpr inline Name operator|(Name a, Name other) { return Name(a.value | other.value); } \
	constexpr inline Name operator&(Name a, Name other) { return Name(a.value & other.value); } \
	constexpr inline Name operator^(Name a, Name other) { return Name(a.value ^ other.value); } \
	constexpr inline Name operator~(Name a) { return Name(~a.value); } \
	\
	constexpr inline Name& operator|=(Name& a, Name other) { a.value |= other.value; return a; } \
	constexpr inline Name& operator&=(Name& a, Name other) { a.value &= other.value; return a; } \
	constexpr inline Name& operator^=(Name& a, Name other) { a.value ^= other.value; return a; } \

#ifndef BENCH_NO_NONPREFIXED_MACROS
#define ENUM_FLAGS BENCH_ENUM_FLAGS
#endif

