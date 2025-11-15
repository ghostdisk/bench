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
	// If file exists        - Fail.
	// If file doesn't exist - Create it and open it.
	CREATE_NEW         = 1,

	// If file exists        - Open it and clear it.
	// If file doesn't exist - Create it an dopen it.
	CREATE_ALWAYS      = 2,

	// If file exists        - Open it.
	// If file doesn't exist - Fail.
	OPEN_EXISTING      = 3,

	// If file exists        - Open it.
	// If file doesn't exist - Create it and open it.
	OPEN_ALWAYS        = 4,
};

struct FileStat {
	bool exists = false;
	bool directory = false;
	bool normal_file = false;
};

struct FileHandle {
	static constexpr void* INVALID_VALUE = (void*)-1;

	void* m_value = INVALID_VALUE;

	static FileHandle Open(String path, FileFlags flags, FileCreateDisposition mode);

	I32   Read(I32 size, void* buffer);
	I32   Write(I32 size, const void* buffer);
	I32   ReadAsync(Coroutine* coro, I32 size, void* buffer);
	void  Seek(I32 offset, FileSeek whence);
	U32   Tell();
	void  SetEndOfFile();
	void  Close();

	operator bool() {
		return m_value != INVALID_VALUE;
	}
	operator Writer() const;
};

class File : public FileHandle {
public:
	File();
	File(FileHandle handle);
	File(File&& other);
	File(const File& other) = delete;
	File& operator=(File&& other);
	File& operator=(const File& other) = delete;
	~File();

	static File Open(String path, FileFlags flags, FileCreateDisposition mode);
	static bool ReadEntireFile(String path, void** out_data, U32* out_size);
	static bool ReadEntireFileAsync(Coroutine* coro, String path, void** out_data, U32* out_size);
	static FileStat Stat(String path);

	static File StdIn;
	static File StdOut;
	static File StdErr;
};

bool PollFileEvents();

};

#define enum_flags BENCH_enum_flags