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

void Format(const Writer& writer, const U8& value);
void Format(const Writer& writer, const U16& value);
void Format(const Writer& writer, const U32& value);
void Format(const Writer& writer, const U64& value);
void Format(const Writer& writer, const I8& value);
void Format(const Writer& writer, const I16& value);
void Format(const Writer& writer, const I32& value);
void Format(const Writer& writer, const I64& value);
void Format(const Writer& writer, const float& value);
void Format(const Writer& writer, const double& value);
void Format(const Writer& writer, const bool& value);
void Format(const Writer& writer, const void* value);

}