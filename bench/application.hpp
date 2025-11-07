#pragma once
#include <bench/common.hpp>
#include <bench/window.hpp>

namespace bench {

struct InitApplicationOptions {
	String title = {};
};

bool InitApplication(const InitApplicationOptions& options);
void BeginFrame();
void EndFrame();
Window GetMainWindow();
void Quit();
bool ShouldQuit();

}