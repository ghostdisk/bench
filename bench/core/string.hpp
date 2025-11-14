#pragma once
#include <bench/core/common.hpp>

namespace bench {

struct String;
struct Writer;
class HeapString;

struct String {
	U8* data;
	U32 length;

	String() {
		this->data = nullptr;
		this->length = 0;
	}

	String(U8* data, U32 length) {
		this->data = data;
		this->length = length;
	}

	String(const char* cstring);

	operator bool() {
		return length > 0;
	}

	bool operator==(String other) const;
	bool operator==(const char* cstring) const;

	bool Cut(U8 byte, String& a, String& b) const;
	String Trim() const;
};

class HeapString {
public:
	String m_string = {};

	HeapString();
	HeapString(const char* cstring);
	HeapString(const wchar_t* wide_string);
	HeapString(String string);
	HeapString(const HeapString& other);
	HeapString(HeapString&& other);
	HeapString& operator=(const HeapString& other);
	HeapString& operator=(HeapString&& other);
	~HeapString();
	operator String() const;
	bool operator==(String other) const;
	bool operator==(const char* cstring) const;
};


void Fmt(const Writer& writer, String str);

}