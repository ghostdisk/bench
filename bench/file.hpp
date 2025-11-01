#pragma once
#include <bench/common.hpp>

namespace bench {

class CoroutineHandle;

struct File {
	void* handle = nullptr;
};

File FileOpen(const char* path);
I32 FileReadAsync(const CoroutineHandle& coro, File& file, I32 size, void* buffer);
void PollFileEvents();

};