#pragma once
#include <bench/core/string.hpp>
#include <bench/core/ini.hpp>
#include <bench/core/file.hpp>

namespace bench {

class Project {
public:
	IniFile ini;

	static Project* Load(String path, FileCreateDisposition create_disposition);

	String GetName();
	void SetName(String name);
	void Save();
};


};