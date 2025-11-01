#include <bench/common.hpp>
#include <Windows.h>

namespace bench {

void AssertAlways(bool condition, const char* fail_message) {
	if (!condition) {
		MessageBoxA(nullptr, fail_message, "Fatal Error", MB_ICONERROR | MB_OK);
		ExitProcess(1);
	}
}

}