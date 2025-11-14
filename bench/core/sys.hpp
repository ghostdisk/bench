#pragma once
#include <bench/core/common.hpp>

typedef struct HINSTANCE__* HINSTANCE;

namespace bench {

BENCH_ENUM_FLAGS(VirtualMemoryProtection, U8,
	NONE = 0x00,
	READ = 0x01,
	WRITE = 0x02,
	EXECUTE = 0x04,
);

enum class VirtualAllocType {
	RESERVE,
	COMMIT,
};

enum class VirtualFreeType {
	DECOMMIT,
	RELEASE,
};

/**
 * Allocate a chunk of virtual memory.
 */
void* VirtualAlloc(void* address, U32 size, VirtualAllocType type, VirtualMemoryProtection protection);

/**
 * Free a chunk of virtual memory.
 * `size` MUST be the same `size` that was passed to VirtualAlloc!
 */
void VirtualFree(void* address, U32 size, VirtualFreeType type);

extern HINSTANCE g_hinstance;

}