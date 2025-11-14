#include <bench/core/arena.hpp>
#include <bench/core/string.hpp>
#include <bench/windows.h>
#include <stdlib.h>
#include <string.h>

namespace bench {

static Arena g_frame_arenas[NUM_FRAME_ARENAS] = {};
static Arena g_scratch_arena;
static U32 g_current_frame_arena_index = 0;

void InitArenas() {
	for (U32 i = 0; i < NUM_FRAME_ARENAS; i++) {
		g_frame_arenas[i].Init();
	}
	g_scratch_arena.Init();
}

void RotateScratchArenas() {
	g_current_frame_arena_index++;
	if (g_current_frame_arena_index >= NUM_FRAME_ARENAS)
		g_current_frame_arena_index = 0;
}


ScratchArenaView::ScratchArenaView(Arena& arena) : arena(arena) {
	this->reset_point = arena.GetResetPoint();
}

ScratchArenaView::~ScratchArenaView() {
	this->arena.Reset(this->reset_point);
}

ScratchArenaView Arena::Scratch() {
	return ScratchArenaView(g_scratch_arena);
}

Arena* Arena::CreateAndGetPtr() {
	Arena arena = {};
	arena.Init();
	Arena* copy_onto_itself = arena.New<Arena>();
	*copy_onto_itself = arena;
	return copy_onto_itself;
}

Arena& Arena::FrameArena() {
	return g_frame_arenas[g_current_frame_arena_index];
}

void Arena::Init() {
	this->start = (U8*)malloc(ARENA_SIZE);
	this->end = this->start + ARENA_SIZE;
	this->head = this->start;

	AssertAlways(this->start, "Out of memory");
}

void Arena::Destroy() {
	if (this->start) {
		free(this->start);
	}
	*this = {};
}

void Arena::AlignHead(U32 alignment) {
	uintptr_t mask = uintptr_t(alignment - 1);
	this->head = (U8*)((uintptr_t(this->head) + mask) & (~mask));
}

void* Arena::Allocate(U32 length) {
	U8* ptr = this->head;
	this->head += length;
	AssertAlways(this->head <= this->end, "Arena overflow");
	return ptr;
}

void* Arena::Allocate(U32 length, U32 alignment) {
	this->AlignHead(alignment);
	return this->Allocate(length);
}

ArenaResetPoint Arena::GetResetPoint() {
	return ArenaResetPoint{this->head};
}

void Arena::Reset(ArenaResetPoint reset_point) {
	this->head = reset_point.head;
}

String Arena::InternString(String string) {
	U8* buffer = (U8*)this->Allocate(string.length);
	memcpy(buffer, string.data, string.length);
	return String(buffer, string.length);
}

char* Arena::InternCString(String string) {
	U8* buffer = (U8*)this->Allocate(string.length + 1);
	memcpy(buffer, string.data, string.length);
	buffer[string.length] = '\0';
	return (char*)buffer;
}

String Arena::InternString(const wchar_t* wide_string) {
	int buffer_size = WideCharToMultiByte(CP_UTF8, 0u, wide_string, -1, nullptr, 0, nullptr, nullptr);
	if (buffer_size) {
		U8* buffer = (U8*)this->Allocate(buffer_size);
		if (WideCharToMultiByte(CP_UTF8, 0u, wide_string, -1, (char*)buffer, buffer_size, nullptr, nullptr))
			return String((const char*)buffer);
		else
			return {};
	}
	else return {};
}

wchar_t* Arena::InternWideCString(String string) {
	int num_utf16_characters = MultiByteToWideChar(CP_UTF8, 0u, (char*)string.data, string.length, nullptr, 0);
	if (num_utf16_characters) {
		wchar_t* wide_buffer = (wchar_t*)this->Allocate((num_utf16_characters + 1) * 2);
		if (MultiByteToWideChar(CP_UTF8, 0u, (char*)string.data, string.length, wide_buffer, num_utf16_characters)) {
			wide_buffer[num_utf16_characters] = L'\0';
			return wide_buffer;
		}
		else {
			return nullptr;
		}
	}
}

}