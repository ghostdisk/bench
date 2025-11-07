#include <bench/application.hpp>
#include <bench/arena.hpp>
#include <bench/file.hpp>
#include <bench/time.hpp>
#include <bench/window.hpp>
#include <bench/coroutine.hpp>
#include <bench/gamesettings.hpp>

namespace bench {

void InitArenas();
void InitWin32();
void InitRenderer();
void RotateScratchArenas();

Window g_main_window = {};
bool g_quit = false;

bool InitApplication(const InitApplicationOptions& options) {
	InitArenas();
	InitWin32();

	GameSettings() = IniFile::Load("settings.ini");

	CreateWindowOptions main_window_options = {};
	main_window_options.title = options.title;
	main_window_options.width = GameSettings().GetInt("window_width", 800);
	main_window_options.height = GameSettings().GetInt("window_height", 600);

	g_main_window = CreateWindow(main_window_options);

	InitRenderer();
	return true;
}

void BeginFrame() {
	RotateScratchArenas();

	bool did_something = false;

	do {
		did_something = false;
		did_something |= PollFileEvents();
		did_something |= ProcessSleepingCoroutines();
		did_something |= PollWindowEvents();
		did_something |= ExecScheduledCoroutines();
	} while (did_something);
}

void EndFrame() {
	GameSettings().Save();
}

Window GetMainWindow() {
	return g_main_window;
}

void Quit() {
	g_quit = true;
}

bool ShouldQuit() {
	return g_quit;
}

}