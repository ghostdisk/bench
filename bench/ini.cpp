#include <bench/ini.hpp>
#include <bench/file.hpp>

namespace bench {

IniFile IniFile::Load(const char* path) {
	void* file_data;
	U32 file_size;
	if (!File::ReadEntireFile(path, &file_data, &file_size))
		return {};

	free(file_data);
}

void IniFile::Save(const char* path) {
}

}
