#pragma once
#include <bench/writer.hpp>
#include <bench/arena.hpp>

namespace bench {

struct ArenaFormatContext {
	Arena* arena = nullptr;
	U8* begin = nullptr;
};

void Format1(const Writer& writer, const U8& value);
void Format1(const Writer& writer, const U16& value);
void Format1(const Writer& writer, const U32& value);
void Format1(const Writer& writer, const U64& value);
void Format1(const Writer& writer, const I8& value);
void Format1(const Writer& writer, const I16& value);
void Format1(const Writer& writer, const I32& value);
void Format1(const Writer& writer, const I64& value);
void Format1(const Writer& writer, const float& value);
void Format1(const Writer& writer, const double& value);
void Format1(const Writer& writer, const bool& value);
void Format1(const Writer& writer, const void* value);
void Format1(const Writer& writer, String value);
void Format1(const Writer& writer, const char* value);

template <typename... Args>
inline void Format(const Writer& writer, Args&&... args);

template <typename Head>
inline void Format(const Writer& writer, Head&& head) {
    Format1(writer, static_cast<Head&&>(head));
}

template <typename Head, typename... Tail>
inline void Format(const Writer& writer, Head&& head, Tail&&... tail) {
    Format1(writer, static_cast<Head&&>(head));
    Format(writer, static_cast<Tail&&>(tail)...);
}

extern WriterVTable g_arena_writer_vtable;

template <typename... Args>
inline String Format(Arena& arena, Args&&... args) {
	ArenaFormatContext context = { &arena, arena.head };
	Writer writer = { &g_arena_writer_vtable, (void*)&context };
	Format(writer, static_cast<Args&&>(args)...);
	return String(context.begin, (U32)(arena.head - context.begin));
}

}