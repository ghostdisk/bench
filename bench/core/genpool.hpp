#pragma once
#include <bench/common.hpp>

namespace bench {

void* GenerationalMalloc(U32 size, U32 alignment);
void GenerationalFree(void* ptr);
U23 GetGeneration(void* ptr);

template <typename T>
struct GenHandle {
	T* value = nullptr;
	U32 generation = 0;
};

}