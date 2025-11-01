#include <bench/file.hpp>
#include <bench/coroutine.hpp>
#include <stdlib.h>

#include <windows.h>
#undef OPEN_EXISTING

namespace bench {

bool File::ReadEntireFile(const char* path, void** out_data, U32* out_size) {
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

}
