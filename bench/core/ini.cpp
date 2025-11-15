#include <bench/core/arena.hpp>
#include <bench/core/file.hpp>
#include <bench/core/string.hpp>
#include <bench/core/ini.hpp>
#include <bench/gamesettings.hpp>
#include <bench/core/sys.hpp>
#include <stdlib.h>
#include <stdio.h>

namespace bench {

IniFile g_settings = {};

IniFile& GameSettings() {
	return g_settings;
}

bool IniFile::Load(String path, bool lock_file, FileCreateDisposition create_disposition) {
	if (m_path) {
		if (m_path == path)
			Destroy();
		else
			return false;
	}
	m_path = path;

	m_file = File::Open(path, FileFlags::READ | FileFlags::WRITE, create_disposition);
	if (!m_file) {
		Destroy();
		return false;
	}

	m_file.Seek(0, FileSeek::END);
	U32 file_size = m_file.Tell();

	void* file_data = malloc(file_size + 1);
	DEFER(free(file_data));

	m_file.Seek(0, FileSeek::START);
	m_file.Read(file_size, file_data);

	String remaining((U8*)file_data, file_size);
	while (remaining) {
		String line;
		remaining.Cut('\n', line, remaining);

		String key, value;
		if (line.Cut('=', key, value)) {
			key = key.Trim();
			value = value.Trim();
			if (key && value)
				SetString(key, value);
		}
	}

	m_file_locked = lock_file;
	if (!m_file_locked) {
		m_file.Close();
	}
	return true;
}

void IniFile::SetString(String key, String value) {
	m_dirty = true;
	for (IniFileEntry& entry : m_entries) {
		if (entry.key == key) {
			entry.value = value;
			return;
		}
	}
	m_entries.Push({ key, value });
}

String IniFile::GetString(String key, String fallback) {
	for (const IniFileEntry& entry : m_entries) {
		if (entry.key == key)
			return entry.value;
	}
	return fallback;
}

bool IniFile::Contains(String key) {
	for (const IniFileEntry& entry : m_entries) {
		if (entry.key == key)
			return true;
	}
	return false;
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
	if (m_dirty) {
		if (!m_file_locked) {
			m_file = File::Open(m_path, FileFlags::WRITE, FileCreateDisposition::OPEN_ALWAYS);
		}
		assert(m_file);

		m_file.Seek(0, FileSeek::START);
		m_file.SetEndOfFile();

		for (const IniFileEntry& entry : this->m_entries) {
			Write(m_file, entry.key);
			Write(m_file, " = ");
			Write(m_file, entry.value);
			Write(m_file, "\r\n");
		}

		if (!m_file_locked) {
			m_file.Close();
		}
		m_dirty = false;
	}
}

void IniFile::Destroy() {
	*this = {};
}

}
