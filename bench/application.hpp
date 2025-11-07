#pragma once
#include <bench/common.hpp>
#include <bench/string.hpp>

namespace bench {

struct Window;

struct InitApplicationOptions {
	String title = {};
};

bool InitApplication(const InitApplicationOptions& options);
void BeginFrame();
void EndFrame();
void Quit();
bool ShouldQuit();
Window GetMainWindow();

}