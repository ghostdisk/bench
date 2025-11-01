#include <bench/application.hpp>
#include <bench/file.hpp>
#include <bench/window.hpp>
#include <bench/coroutine.hpp>

namespace bench {

void InitWin32();
void InitRenderer();

Window g_main_window = {};

bool ApplicationInit(const ApplicationInitOptions& options) {
	InitWin32();
	g_main_window = CreateWindow(options.main_window_options);
	InitRenderer();
	return true;
}

void ApplicationDoFrame() {
	bool did_something = false;
	do {
		did_something = false;
		did_something |= PollFileEvents();
		did_something |= PollWindowEvents();
		did_something |= ExecScheduledCoroutines();
	} while (did_something);
}

}