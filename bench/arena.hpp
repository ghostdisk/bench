#pragma once
#include <bench/common.hpp>
#include <new>

namespace bench {

/**
 * How many frame arenas are in rotation?
 * Memory allocated by a frame arena will live for NUM_FRAME_ARENAS frames
 * Must be at least 2.
 */
static constexpr U32 NUM_FRAME_ARENAS = 3;

static constexpr U32 ARENA_SIZE = (1 << 20);

struct String;
struct ScratchArenaView;

struct ArenaResetPoint {
	U8* head = nullptr;
};

struct Arena {
	U8* start = nullptr;
	U8* end = nullptr;
	U8* head = nullptr;

	void Init();
	void Destroy();
	void AlignHead(U32 alignment);
	void* Allocate(U32 length);
	void* Allocate(U32 length, U32 alignment);
	ArenaResetPoint GetResetPoint();
	void Reset(ArenaResetPoint reset_point);

	String InternString(String string);
	char* InternCString(String string);

	static ScratchArenaView Scratch();
	static Arena& FrameArena();

	template <typename T, typename ...Args>
	T* New(Args&& ...args) {
		T* ptr = (T*)Allocate(sizeof(T), alignof(T));
		new (ptr) T(static_cast<Args&&>(...args));
		return ptr;
	}
};

class ScratchArenaView {
public:
	Arena& arena;
	ArenaResetPoint reset_point;

	ScratchArenaView(Arena& arena);
	~ScratchArenaView();
};

}