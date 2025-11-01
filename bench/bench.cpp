#include <bench/bench.hpp>

namespace bench {

void InitFileIO();

bool BenchInit(const BenchInitOptions& options) {
	InitFileIO();
	return true;
}

}