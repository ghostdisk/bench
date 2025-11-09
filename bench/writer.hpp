#pragma once
#include <bench/common.hpp>

namespace bench {

struct String;

struct WriterVTable {
	I32(*write_proc)(void* userdata, const void* buffer, I32 size);
};

struct Writer {
	WriterVTable* vtable = nullptr;
	void* userdata;

	inline void Write(const void* buffer, I32 size) const {
		this->vtable->write_proc(this->userdata, buffer, size);
	}
};


}