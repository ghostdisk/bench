#pragma once
#include <bench/core/common.hpp>

namespace bench {

/// How many frame arenas are in rotation.
/// Memory allocated by a frame arena will live for NUM_FRAME_ARENAS frames.
/// Must be at least 2.
static constexpr U32 NUM_FRAME_ARENAS = 3;

/// Default size of each arena in bytes (1 MB).
static constexpr U32 ARENA_SIZE = (1 << 20);

struct String;
class ScratchArenaView;

/// Linear allocation arena.
/// Allocates memory in a contiguous block. Supports scoped resets and string interning.
struct Arena {
    /// Start of the arena memory block.
    U8* start = nullptr;

    /// End of the arena memory block.
    U8* end = nullptr;

    /// Current allocation head.
    U8* head = nullptr;

	/// Represents a point in the arena to which it can be reset.
	/// Example:
	/// ```
	/// Arena::ResetPoint rp1 = arena.GetResetPoint();
	/// void* alloc1 = arena.Allocate(100);
	/// Arena::ResetPoint rp2 = arena.GetResetPoint();
	/// void* alloc2 = arena.Allocate(100);
	/// arena.Reset(rp2); // alloc2 is freed!
	/// arena.Reset(rp1); // alloc1 is also freed!
	/// arena.Destroy(); // All memory from the arena is freed! (you could've just done that to begin with!)
	/// ```
	struct ResetPoint {
		/// Current head pointer of the arena at the reset point.
		U8* head = nullptr;
	};

    /// Initialize the arena. Allocates underlying memory.
    void Init();

    /// Destroy the arena and free all memory.
    void Destroy();

    /// Allocate a block of memory of given length.
    /// @param length Number of bytes to allocate.
    /// @return Pointer to the allocated memory.
    void* Allocate(U32 length);

    /// Allocate a block of memory of given length with alignment.
    /// @param length Number of bytes to allocate.
    /// @param alignment Alignment in bytes.
    /// @return Pointer to the allocated memory.
    void* Allocate(U32 length, U32 alignment);

    /// Construct an object of type T in-place in the arena.
    /// @tparam T Type to construct.
    /// @tparam Args Constructor argument types.
    /// @param args Arguments to forward to the constructor.
    /// @return Pointer to the constructed object.
    template <typename T, typename ...Args>
    inline T* New(Args&& ...args) {
        void* ptr = Allocate(sizeof(T), alignof(T));
        new (ptr) T(static_cast<Args&&>(args)...);
        return static_cast<T*>(ptr);
    }

    /// Get a reset point representing the current head.
    /// @return A ResetPoint to which the arena can be reset later.
    ResetPoint GetResetPoint();

    /// Reset the arena to a previous reset point, effectively freeing all allocations after that point.
    /// @param reset_point The reset point to restore.
    void Reset(ResetPoint reset_point);

    /// Copy a `bench::String` into the arena.
    String InternString(String string);

    /// Convert a UTF-16 string into a `bench::String` and copy it onto the arena.
    String InternString(const wchar_t* widestring);

    /// Convert a `bench::String` into a null-terminated UTF-8 string and copy it onto the arena.
    char* InternCString(String string);

    /// Convert a `bench::String` into a null-terminated UTF-16 string and copy it onto thearenaa.
    wchar_t* InternWideCString(String string);

    /// Align the allocation head to the given alignment.
    void AlignHead(U32 alignment);

    /// Get a reference to the temporary scratch arena.
    /// The memory allocated from this Arena 
    static ScratchArenaView Scratch();

    /// Get a reference to the global frame arena.
    /// The memory allocated from it will live for `NUM_FRAME_ARENAS` frames.
    static Arena& FrameArena();

    /// Create an Arena and copy the Arena struct onto itself.
    static Arena* CreateAndGetPtr();

};

/// RAII helper for the temporary scratch arena.
/// 
/// Example:
/// ```
/// {
///     ScratchArenaView scratch = Arena::Scratch();
///     void* buf = scratch.arena.Allocate(100); // allocate some memory
/// } // <- scratch goes out of scope here, the memory is freed.
/// ```
class ScratchArenaView {
public:
    /// Reference to the underlying arena.
    Arena& arena;

    /// The reset point captured at construction.
    Arena::ResetPoint reset_point;

    /// Construct a scratch view, capturing the current head of the arena.
    /// @param arena Arena to allocate from.
    ScratchArenaView(Arena& arena);

    /// Destructor. Resets the arena to the captured reset point.
    ~ScratchArenaView();
};

} // namespace bench
