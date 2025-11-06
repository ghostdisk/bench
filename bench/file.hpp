#pragma once
#include <bench/common.hpp>
#include <bench/utils/flags.hpp>

namespace bench {

struct Coroutine;

enum class FileSeek {
	START,
	CURRENT,
	END,
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

struct File {
	void* handle = nullptr;

	static File Open(const char* path, FileFlags flags, FileCreateDisposition mode);
	static bool ReadEntireFile(const char* path, void** out_data, U32* out_size);

	I32 Read(I32 size, void* buffer);
	I32 Write(I32 size, const void* buffer);

	I32 ReadAsync(Coroutine* coro, I32 size, void* buffer);
	void Seek(I32 offset, FileSeek whence);
	U32 Tell();
	void Close();

	operator bool() {
		return (U32)handle != 0xFFFFFFFF;
	}
};

bool PollFileEvents();

};