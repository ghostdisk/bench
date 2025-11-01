#pragma once
#include <vector>
#include <string>

namespace bench {

struct IniFileEntry {
	std::string key;
	std::string value;
};

struct IniFile {
	const char* path;
	std::vector<IniFileEntry> entries;

	static IniFile Load(const char* path);
	void Save(const char* path);

	std::string Get(std::string section, std::string key);
	void Set(std::string section, std::string key, std::string value);
};

}