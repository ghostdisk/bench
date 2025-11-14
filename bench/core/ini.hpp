#pragma once
#include <bench/core/string.hpp>
#include <bench/core/arraylist.hpp>

namespace bench {

class IniFileEntry {
public:
	HeapString key;
	HeapString value;
};

class IniFile {
	ArrayList<IniFileEntry> entries;
public:
	HeapString path;
	bool dirty = false;

	static IniFile Load(String path);
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