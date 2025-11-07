#include <bench/arena.hpp>
#include <bench/ini.hpp>
#include <bench/file.hpp>
#include <bench/string.hpp>
#include <bench/utils/defer.hpp>
#include <bench/gamesettings.hpp>

namespace bench {

IniFile g_settings = {};

IniFile& GameSettings() {
	return g_settings;
}

IniFile IniFile::Load(const char* path) {
	IniFile ini = {};
	ini.path = path;

	void* file_data;
	U32 file_size;
	if (!File::ReadEntireFile(path, &file_data, &file_size))
		return ini;
	DEFER(free(file_data));

	String remaining((U8*)file_data, file_size);

	while (remaining) {
		String line;
		remaining.Cut('\n', line, remaining);

		String key, value;
		if (line.Cut('=', key, value)) {
			key = key.Trim();
			value = value.Trim();
			if (key && value)
				ini.SetString(key, value);
		}
	}

	return ini;
}

void IniFile::SetString(String key, String value) {
	this->entries[key.to_std_string()] = value.to_std_string();
	dirty = true;
}

String IniFile::GetString(String key, String fallback) {
	auto it = this->entries.find(key.to_std_string());
	return it != this->entries.end() ? it->second : fallback;
}

bool IniFile::Contains(String key) {
	auto it = this->entries.find(key.to_std_string());
	return it != this->entries.end();
}

I32 IniFile::GetInt(String key, I32 fallback) {
	ScratchArenaView scratch = Arena::Scratch();
	
	const char* value_zstr = scratch.arena.InternCString(GetString(key, "#"));
	char* endptr = nullptr;
	I32 value = strtol(value_zstr, &endptr, 10);

	if (value_zstr == endptr || *endptr != '\0')
		return fallback;

	return value;
}

void IniFile::SetInt(String key, I32 value) {
	char value_str[32];
	snprintf(value_str, sizeof(value_str), "%d", value);

	this->SetString(key, value_str);
}

bool IniFile::GetBool(String key, bool fallback) {
	String str = this->GetString(key, "");
	if (str == "true")
		return true;
	else if (str == "false")
		return false;
	else
		return fallback;
}

void IniFile::SetBool(String key, bool value) {
	this->SetString(key, value ? "true" : "false");
}

static void Write(File& file, String str) {
	file.Write(str.length, str.data);
}

void IniFile::Save() {
	if (dirty) {
		File out = File::Open(path.c_str(), FileFlags::WRITE, FileCreateDisposition::CREATE_ALWAYS);

		for (const auto& entry : this->entries) {
			Write(out, entry.first);
			Write(out, " = ");
			Write(out, entry.second);
			Write(out, "\n");
		}

		out.Close();
		dirty = false;
	}
}

}
