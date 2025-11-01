#pragma once
#include <bench/common.hpp>
#include <bench/window.hpp>

namespace bench {

struct ApplicationInitOptions {
	U32 struct_size = sizeof(ApplicationInitOptions);
	CreateWindowOptions main_window_options = {};
};

bool ApplicationInit(const ApplicationInitOptions& options);
void ApplicationDoFrame();
Window GetMainWindow();

}