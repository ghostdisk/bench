#pragma once
#include <bench/core/common.hpp>

typedef struct HINSTANCE__* HINSTANCE;

namespace bench {

struct Arena;
struct String;

/// Memory protection access bitmask
BENCH_ENUM_FLAGS(VirtualMemoryProtection, U8,
	/// If no other flags are set, memory range will be unaccessible.
	NONE = 0x00,    

	/// Memory range will have readable.
	READ = 0x01,    

	/// Memory range will have writable. On x86 you can't have write-only memory, you need READ as well!
	WRITE = 0x02,   

	/// Memory range will be executable.
	EXECUTE = 0x04, 
);

enum class VirtualAllocType {
	RESERVE, ///< Reserve a virtual-memory range, which 
	COMMIT,  ///< Allocate actual physical memory to back a virtual-memory range
};

enum class VirtualFreeType {
	DECOMMIT, ///< Release the physical memory range at a virtual range, but keep the virtual mapping reserved.
	RELEASE,  ///< Free the virtual memory range and its backing physical memory.
};

/// Allocate a chunk of virtual memory.
/// @param address    Optional virtual memory address. If unset, 
/// @param size       Size of virtual memory to reserve/commit. Will be rounded up to page size.
/// @param type       VirtualAllocType::RESERVE or VirtualAllocType::COMMIT.
/// @param protection Protection constants for the memory range
void* VirtualAlloc(void* address, U32 size, VirtualAllocType type, VirtualMemoryProtection protection);

// Free a chunk of virtual memory.
// @param address The address to decommit/release
// @param size    This MUST be the same `size` that was passed to VirtualAlloc!
// @param type    VirtualFreeType::DECOMMIT or VirtualFreeType::RELEASE.
void VirtualFree(void* address, U32 size, VirtualFreeType type);

/// Get the current working directory. The string be allocated into the passed Arena.
String GetCurrentDirectory(Arena& arena);

/// Convert a possibly relative file path to an absolute path. The absolute path will be allocated into the passed Arena.
String GetAbsolutePath(Arena& arena, String path);

/// The main executable's HINSTANCE.
extern HINSTANCE g_hinstance;

}