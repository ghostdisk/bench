#pragma once
#include <bench/core/common.hpp>

namespace bench {

struct Writer;

/// Non-Owning string view.
/// 
/// `bench::String` is non-owning, UTF-8, non-zero-terminated string view.
/// It has two register-sized members - data and length.
///
/// It's usually passed to functions by value. If a function accepts a String
/// by value, the caller must guarentee, that the String will live until
/// the function returns (this basically means that you won't delete it from another thread).
/// 
/// String does not provide a storage mechanism. If you wish store a string for long-term usage
/// you must copy it onto memory you own:
/// - You can use HeapString, a class similar to the STL std::string.
/// - You can copy it onto an Arena via `Arena::InternString` - the resulting string will live until the Arena is reset or destroyed.
///
/// For UTF-16 string support see `Arena::InternString` and `Arena::InternWideCString`.
/// The idiomatic pattern when working with Windows APIs  is to do all string processing with
/// `bench::String` functionality, and convert to a wide-string on a scratch Arena right before
/// 
struct String {
    /// Pointer to the UTF-8 or raw byte data. Not null-terminated.
    U8* data;

    /// Number of bytes in the string.
    U32 length;

    /// Default constructor for an empty String (data = nullptr, length = 0)
    String() {
        data = nullptr;
        length = 0;
    }

    /// Construct a String from raw data and explicit length. This does NOT copy the data and length.
    String(U8* data, U32 length) {
        this->data = data;
        this->length = length;
    }

    /// Construct a string view from a null-terminated C string.
    /// @param cstring Null-terminated ASCII/UTF-8 string. This will be casted to `U8*` for the `data` memer. Length is computed via `strlen`.
    String(const char* cstring);

    /// Check whether the string is non-empty.
    operator bool() const {
        return length > 0;
    }

    /// Compare two strings for equality (byte-wise).
    bool operator==(String other) const;

    /// Compare this string with a null-terminated C string.
    bool operator==(const char* cstring) const;

    /// Split the string into two parts around a delimiter byte.
    /// @param byte Delimiter byte.
    /// @param a Output: substring before `byte`.
    /// @param b Output: substring after `byte`.
    /// @return `true` if delimiter was found; `false` otherwise.
    bool Cut(U8 byte, String& a, String& b) const;

    /// Return a trimmed substring without leading or trailing whitespace.
    String Trim() const;
};

/// std::string-like Heap-allocated RAII wrapper for `bench::String`.
///
/// This is conceptually very similar to the standard std::string - it stores a heap-allocated `bench::String`
/// that is freed once the string goes out of scope.
/// 
/// `bench::HeapString` can be easily constructed from a `bench::String` and implicitly casts down to `bench::String`
/// so you can easily pass it to all functions that expect a `bench::String`.
class HeapString {
public:
    /// The actual string.
    String m_string = {};

    /// Construct an empty heap string.
    HeapString();

    /// Construct a HeapString by copying a C string.
    HeapString(const char* cstring);

    /// Construct a HeapString by converting from a wide (UTF-16) string.
    /// Useful when dealing with Windows APIs.
    HeapString(const wchar_t* wide_string);

    /// Construct a heap string by copying from an existing String view.
    /// @param string Source string view (data is copied onto the heap).
    HeapString(String string);

    // Copy constructor.
    HeapString(const HeapString& other);

    // Move constructor.
    HeapString(HeapString&& other);

    // Copy assignment operator.
    HeapString& operator=(const HeapString& other);

    // Move assignment operator.
    HeapString& operator=(HeapString&& other);

    /// Destroy the owned string buffer.
    ~HeapString();

    /// Implicit conversion to a String view (returns m_string).
    operator String() const;

    /// Check whether the heap string is non-empty.
    explicit operator bool() const;

    /// Compare with another String for equality.
    bool operator==(String other) const;

    /// Compare with a null-terminated C string for equality.
    bool operator==(const char* cstring) const;
};

/// Format a string into a Writer.
/// @param writer Output writer object.
/// @param str String to format.
void Fmt(const Writer& writer, String str);

} // namespace bench
