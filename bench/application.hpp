#pragma once
#include <bench/core/common.hpp>
#include <bench/core/string.hpp>

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

}