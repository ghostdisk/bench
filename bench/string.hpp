#pragma once
#include <bench/common.hpp>
#include <string.h>
#include <string>

namespace bench {

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
		this->data = (U8*)cstring;
		this->length = strlen(cstring);
	}

	String(const std::string& std_string) {
		this->data = (U8*)std_string.data();
		this->length = std_string.size();
	}

	operator bool() {
		return length > 0;
	}

	bool Cut(U8 byte, String& a, String& b);
	String Trim();
	std::string to_std_string();
};

}