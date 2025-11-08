#pragma once
#include <bench/common.hpp>

namespace bench {

struct WriterVTable {
	I32(*write)(void* userdata, const void* buffer, I32 size);
};

struct Writer {
	WriterVTable* vtable = nullptr;
	void* userdata;
};

}