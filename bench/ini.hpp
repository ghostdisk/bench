#pragma once
#include <bench/string.hpp>
#include <bench/utils/arraylist.hpp>

namespace bench {

struct IniFileEntry {
	String key;
	String value;
};

class IniFile {
	ArrayList<IniFileEntry> entries;
public:
	String path;
	bool dirty = false;

	static IniFile Load(const char* path);
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