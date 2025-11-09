#pragma once
#include <bench/common.hpp>

namespace bench {

/**
 * How many frame arenas are in rotation?
 * Memory allocated by a frame arena will live for NUM_FRAME_ARENAS frames
 * Must be at least 2.
 */
static constexpr U32 NUM_FRAME_ARENAS = 3;

static constexpr U32 ARENA_SIZE = (1 << 20);

struct String;
class ScratchArenaView;

struct ArenaResetPoint {
	U8* head = nullptr;
};

struct Arena {
	U8* start = nullptr;
	U8* end = nullptr;
	U8* head = nullptr;

	void Init();
	void Destroy();

	void* Allocate(U32 length);
	void* Allocate(U32 length, U32 alignment);

	template <typename T, typename ...Args>
	inline T* New(Args&& ...args) {
		void* ptr = Allocate(sizeof(T), alignof(T));
		new (ptr) T(static_cast<Args&&>(args)...);
		return static_cast<T*>(ptr);
	}

	ArenaResetPoint GetResetPoint();
	void Reset(ArenaResetPoint reset_point);

	String InternString(String string);
	char* InternCString(String string);

	void AlignHead(U32 alignment);

	static ScratchArenaView Scratch();
	static Arena& FrameArena();

	/**
	 * Arena::CreateAndGetPtr initializes an arena, copies the arena struct onto its own memory,
	 * and returns that pointer.
	 */
	static Arena* CreateAndGetPtr();
};

class ScratchArenaView {
public:
	Arena& arena;
	ArenaResetPoint reset_point;

	ScratchArenaView(Arena& arena);
	~ScratchArenaView();
};

}