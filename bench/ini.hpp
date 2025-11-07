#pragma once
#include <bench/string.hpp>
#include <unordered_map>

namespace bench {

struct IniFile {
	std::string path;
	std::unordered_map<std::string, std::string> entries;
	bool dirty = false;

	static IniFile Load(const char* path);
	void Save();

	bool Contains(String key);

	String GetString(String key, String fallback = {});
	void SetString(String key, String value);

	I32 GetInt(String key, I32 fallback = 0);
	void SetInt(String key, I32 value);

	bool GetBool(String key, bool fallback = false);
	void SetBool(String key, bool value);

};

}