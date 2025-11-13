#include <bench/core/string.hpp>
#include <bench/core/format.hpp>
#include <stdio.h>
#include <string.h>

namespace bench {

WriterVTable g_arena_writer_vtable = {
	[](void* userdata, const void* buffer, I32 size) {
		ArenaFormatContext* context = (ArenaFormatContext*)userdata;
		void* head = context->arena->Allocate(size);
		memcpy(head, buffer, size);
        return size;
	},
};

void Format1(const Writer& writer, const U64& value) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%llu", value);
    writer.Write(buffer, length);
}

void Format1(const Writer& writer, const I64& value) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%lld", value);
    writer.Write(buffer, length);
}

void Format1(const Writer& writer, const U32& value) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%u", value);
    writer.Write(buffer, length);
}

void Format1(const Writer& writer, const I32& value) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%d", value);
    writer.Write(buffer, length);
}

void Format1(const Writer& writer, const double& value) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%f", value);
    writer.Write(buffer, length);
}

void Format1(const Writer& writer, const bool& value) {
    String str = String(value ? "true" : "false");
    writer.Write(str.data, str.length);
}

void Format1(const Writer& writer, const void* value) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%p", value);
    writer.Write(buffer, length);
}

void Format1(const Writer& writer, const float& value) {
    Format1(writer, (double)value);
}

void Format1(const Writer& writer, String value) {
    writer.Write(value.data, value.length);
}

void Format1(const Writer& writer, const char* value) {
    Format1(writer, String(value));
}

void Format1(const Writer& writer, U8 value) {
    Format1(writer, (U32)value);
}

void Format1(const Writer& writer, I8 value) {
    Format1(writer, (I32)value);
}

void Format1(const Writer& writer, U16 value) {
    Format1(writer, (U32)value);
}

void Format1(const Writer& writer, I16 value) {
    Format1(writer, (I32)value);
}

}