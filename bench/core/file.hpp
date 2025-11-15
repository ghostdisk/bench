#pragma once
#include <bench/core/common.hpp>

namespace bench {

struct String;
struct Coroutine;
struct Writer;

/// File seek origin used with FileHandle::Seek.
enum class FileSeek {
	/// Seek relative to the beginning of the file.
	START,

	/// Seek relative to the current file position.
	CURRENT,

	/// Seek relative to the end of the file.
	END,
};

/// Flags for `File::Open`.
BENCH_ENUM_FLAGS(FileFlags, U32,
	/// Open file for reading.
	READ   = 1, 
	/// Open file for writing.
	WRITE  = 2, 
	/// Open file in async mode. 
	ASYNC  = 4, 
);

/// Specifies how File::Open behaves when opening or creating a file.
/// 
/// These values match the Win32 API creation disposition values.
/// See Microsoft docs:
/// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
enum class FileCreateDisposition {

	/// If the file exists:  **Fail**.  
	/// If it does not:      Create it and open it.
	CREATE_NEW         = 1,

	/// If the file exists:  Open it and truncate it to zero length.  
	/// If it does not:      Create it and open it.
	CREATE_ALWAYS      = 2,

	/// If the file exists:  Open it.  
	/// If it does not:      **Fail**.
	OPEN_EXISTING      = 3,

	/// If the file exists:  Open it.  
	/// If it does not:      Create it and open it.
	OPEN_ALWAYS        = 4,
};

/// Result of querying a file's status with File::Stat.
struct FileStat {
	/// True if the path exists.
	bool exists = false;

	/// True if the path refers to a directory.
	bool directory = false;

	/// True if the path refers to a regular non-directory file.
	bool normal_file = false;
};

/// Low-level OS file handle.  
/// This is a wrapper around the OS-level file handle.
/// Usually you should use the higher-level `bench::File` class wrapper, which provides RAII/Move semantics.
struct FileHandle {
	/// Invalid file handle value used by platform API.
	static constexpr void* INVALID_VALUE = (void*)-1;

	/// OS file handle.
	void* m_value = INVALID_VALUE;

	/// Open a file with the given path, flags, and creation mode.
	/// @param path Path to the file.
	/// @param flags Read/write/async flags.
	/// @param mode File creation/opening behavior.
	/// @return A FileHandle. Check with `if (handle)` for validity.
	static FileHandle Open(String path, FileFlags flags, FileCreateDisposition mode);

	/// Read bytes from the file.
	/// @param size Number of bytes to read.
	/// @param buffer Destination buffer.
	/// @return Number of bytes actually read, or negative on error.
	I32 Read(I32 size, void* buffer);

	/// Write bytes to the file.
	/// @param size Number of bytes to write.
	/// @param buffer Source buffer.
	/// @return Number of bytes actually written, or negative on error.
	I32 Write(I32 size, const void* buffer);

	/// Issue an asynchronous read operation.
	/// @param coro Handle to the current coroutine. It will be blocked until the read completes.
	/// @param size Number of bytes to read.
	/// @param buffer Destination buffer.
	/// @return Number of bytes read, or negative on error.
	I32 ReadAsync(Coroutine* coro, I32 size, void* buffer);

	/// Seek to a new position in the file.
	/// @param offset Offset in bytes.
	/// @param whence Starting reference (beginning, current, end).
	void Seek(I32 offset, FileSeek whence);

	/// Get the current file position.
	/// @return Byte offset from file start.
	U32 Tell();

	/// Truncate or extend the file at the current position.
	void SetEndOfFile();

	/// Close the handle. No-op if already invalid.
	void Close();

	/// @return True if the handle is valid.
	operator bool() {
		return m_value != INVALID_VALUE;
	}

	/// Convert this file handle into a Writer for formatted output.
	operator Writer() const;
};

/// High-level RAII wrapper around FileHandle.
/// 
/// Automatically closes the file on destruction and provides convenient
/// helpers for opening files, reading them fully, and checking file status.
class File : public FileHandle {
public:
	/// Create an invalid file object.
	File();

	/// Construct from an existing FileHandle.
	File(FileHandle handle);

	/// Move constructor.
	File(File&& other);

	/// Move assignment.
	File& operator=(File&& other);

	/// This class is not copyable.
	File(const File& other) = delete;

	/// This class is not copyable.
	File& operator=(const File& other) = delete;

	/// The File is closed in destructor.
	~File();

	/// Open a file.
	/// @param path File path.
	/// @param flags Read/write/async flags.
	/// @param mode Creation/opening behavior.
	/// @return A File object.
	static File Open(String path, FileFlags flags, FileCreateDisposition mode);

	/// Read an entire file into memory.
	/// Memory is heap-allocated; caller must free with `free()`.
	/// @param path File to read.
	/// @param out_data Filled with pointer to allocated memory.
	/// @param out_size Filled with file size in bytes.
	/// @return True on success.
	static bool ReadEntireFile(String path, void** out_data, U32* out_size);

	/// Read an entire file into memory asynchronously.
	/// @param coro The current coroutine. It will be resumed when the read finishes.
	/// @param path File path.
	/// @param out_data Pointer to receive allocated memory.
	/// @param out_size Pointer to receive size.
	/// @return True on success.
	static bool ReadEntireFileAsync(Coroutine* coro, String path, void** out_data, U32* out_size);

	/// Query file or directory status.
	/// @param path Path to check.
	/// @return FileStat describing existence and type.
	static FileStat Stat(String path);

	/// Standard input (readonly).
	static File StdIn;

	/// Standard output (writeonly).
	static File StdOut;

	/// Standard error (writeonly).
	static File StdErr;
};

/// Polls async file I/O events (e.g. completions).
/// This is called internally by the SDK.
bool PollFileEvents();

} // namespace bench

