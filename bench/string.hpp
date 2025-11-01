#pragma once
#include <bench/common.hpp>
#include <string.h>

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
};

}