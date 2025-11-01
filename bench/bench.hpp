#pragma once
#include <bench/common.hpp>

namespace bench {

struct BenchInitOptions {
	U32 struct_size = sizeof(BenchInitOptions);
};

bool BenchInit(const BenchInitOptions& options);

}