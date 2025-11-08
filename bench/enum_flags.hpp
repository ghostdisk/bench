#pragma once

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

