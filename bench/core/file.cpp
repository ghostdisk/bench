#include <bench/core/file.hpp>
#include <bench/core/string.hpp>
#include <bench/core/writer.hpp>
#include <bench/core/coroutine.hpp>
#include <stdlib.h>

namespace bench {

File File::StdIn;
File File::StdOut;
File File::StdErr;

bool File::ReadEntireFile(String path, void** out_data, U32* out_size) {
	*out_data = nullptr;
	if (out_size) *out_size = 0;

	File file = File::Open(path, FileFlags::READ, FileCreateDisposition::OPEN_EXISTING);
	if (!file)
		return false;

	file.Seek(0, FileSeek::END);
	I32 size = file.Tell();
	file.Seek(0, FileSeek::START);

	char* data = (char*)malloc(size + 1);
	file.Read(size, data);
	data[size] = '\0';

	file.Close();

	*out_data = data;
	if (out_size) *out_size = size;

	return true;
}

bool File::ReadEntireFileAsync(Coroutine* coro, String path, void** out_data, U32* out_size) {
	*out_data = nullptr;
	if (out_size) *out_size = 0;

	File file = File::Open(path, FileFlags::READ, FileCreateDisposition::OPEN_EXISTING);
	if (!file)
		return false;

	file.Seek(0, FileSeek::END);
	I32 size = file.Tell();
	file.Seek(0, FileSeek::START);

	char* data = (char*)malloc(size + 1);
	file.ReadAsync(coro, size, data);
	data[size] = '\0';

	file.Close();

	*out_data = data;
	if (out_size) *out_size = size;

	return true;
}

File::operator Writer() const {
	static WriterVTable writer_vtable = {
		[](void* userdata, const void* buffer, I32 size) {
			File* file = (File*)userdata;
			return file->Write((I32)size, buffer);
		},
	};
	return Writer{ &writer_vtable, (void*)this };
}

}
