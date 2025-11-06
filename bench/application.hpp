#pragma once
#include <bench/common.hpp>
#include <bench/window.hpp>

namespace bench {

struct InitApplicationOptions {
	U32 struct_size = sizeof(InitApplicationOptions);
	String title = {};
};

bool InitApplication(const InitApplicationOptions& options);
void BeginFrame();
void EndFrame();
Window GetMainWindow();

}