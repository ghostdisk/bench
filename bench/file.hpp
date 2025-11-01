#pragma once
#include <bench/common.hpp>
#include <bench/utils/flags.hpp>

namespace bench {

struct Coroutine;

struct File {
	void* handle = nullptr;
};

BENCH_flags(FileFlags, U32,
	READ   = 1,
	WRITE  = 2,
	ASYNC  = 4,
);

enum class FileCreateDisposition {
	CREATE_NEW         = 1,
	CREATE_ALWAYS      = 2,
	OPEN_EXISTING      = 3,
	OPEN_ALWAYS        = 4,
	TRUNCATE_EXISTING  = 5,
};

File FileOpen(const char* path, FileFlags flags, FileCreateDisposition mode);
I32 FileReadAsync(Coroutine* coro, File& file, I32 size, void* buffer);
bool PollFileEvents();

};