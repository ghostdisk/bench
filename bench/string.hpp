#pragma once
#include <bench/common.hpp>
#include <string.h>

namespace bench {

struct Writer;

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

	String(const char* cstring) {
		assert(cstring);
		this->data = (U8*)cstring;
		this->length = strlen(cstring);
	}

	operator bool() {
		return length > 0;
	}

	bool operator==(String other) const;
	bool operator==(const char* cstring) const;

	bool Cut(U8 byte, String& a, String& b) const;
	String Trim() const;

	String CopyToHeap() const;
	void FreeFromHeap();
};

void Fmt(const Writer& writer, String str);

}