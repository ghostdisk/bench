#pragma once

#define BENCH_flags(Name, UnderlyingType, ...) \
    struct Name { \
        UnderlyingType value = 0; \
        \
        enum Bits : UnderlyingType { \
            __VA_ARGS__ \
        }; \
        \
        Name() = default; \
        Name(UnderlyingType value) : value(value) {} \
        \
        explicit operator bool() const { return value != 0; } \
        explicit operator UnderlyingType() const { return value; } \
        \
        Name operator|(Name other) const { return Name(value | other.value); } \
        Name operator&(Name other) const { return Name(value & other.value); } \
        Name operator^(Name other) const { return Name(value ^ other.value); } \
        Name operator~() const { return Name(~value); } \
        \
        Name& operator|=(Name other) { value |= other.value; return *this; } \
        Name& operator&=(Name other) { value &= other.value; return *this; } \
        Name& operator^=(Name other) { value ^= other.value; return *this; } \
        \
        bool operator==(Name other) const { return value == other.value; } \
        bool operator!=(Name other) const { return value != other.value; } \
    };