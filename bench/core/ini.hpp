#pragma once
#include <bench/core/string.hpp>
#include <bench/core/arraylist.hpp>
#include <bench/core/file.hpp>

namespace bench {

class IniFileEntry {
public:
	HeapString key;
	HeapString value;
};

class IniFile {
	ArrayList<IniFileEntry> m_entries = {};
	File m_file = {};
	bool m_file_locked = {};
	HeapString m_path = {};
	bool m_dirty = false;

public:
	bool Load(String path, bool lock_file, FileCreateDisposition create_disposition);
	void Save();
	void Destroy();

	bool Contains(String key);

	String GetString(String key, String fallback = {});
	void SetString(String key, String value);

	I32 GetInt(String key, I32 fallback = 0);
	void SetInt(String key, I32 value);

	bool GetBool(String key, bool fallback = false);
	void SetBool(String key, bool value);

};

}