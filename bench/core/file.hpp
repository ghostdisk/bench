#pragma once
#include <bench/core/common.hpp>

namespace bench {

struct String;
struct Coroutine;
struct Writer;

enum class FileSeek {
	START,
	CURRENT,
	END,
};

BENCH_ENUM_FLAGS(FileFlags, U32,
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

	static File Open(String path, FileFlags flags, FileCreateDisposition mode);
	static bool ReadEntireFile(String path, void** out_data, U32* out_size);
	static bool ReadEntireFileAsync(Coroutine* coro, String path, void** out_data, U32* out_size);

	I32 Read(I32 size, void* buffer);
	I32 Write(I32 size, const void* buffer);

	I32 ReadAsync(Coroutine* coro, I32 size, void* buffer);
	void Seek(I32 offset, FileSeek whence);
	U32 Tell();
	void Close();

	operator bool() {
		return (U32)handle != 0xFFFFFFFF;
	}

	operator Writer() const;

	static File StdIn;
	static File StdOut;
	static File StdErr;
};

bool PollFileEvents();

};

#define enum_flags BENCH_enum_flags