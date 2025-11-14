#include <bench/application.hpp>
#include <bench/core/arena.hpp>
#include <bench/core/file.hpp>
#include <bench/core/time.hpp>
#include <bench/core/coroutine.hpp>
#include <bench/core/window.hpp>
#include <bench/gamesettings.hpp>
#include <bench/core/sys.hpp>

namespace bench {

void InitArenas();
void InitWin32();
void RotateScratchArenas();

bool g_quit = false;

bool InitApplication(const InitApplicationOptions& options) {
	InitArenas();
	InitWin32();

	ScratchArenaView scratch = Arena::Scratch();

	String settings_path = options.settings_path ? options.settings_path : String("settings.ini");
	settings_path = GetAbsolutePath(scratch.arena, settings_path);

	GameSettings() = IniFile::Load(settings_path);
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

void Quit() {
	g_quit = true;
}

bool ShouldQuit() {
	return g_quit;
}

}