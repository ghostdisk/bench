#include <bench/core/window.hpp>
#include <bench/core/coroutine.hpp>
#include <bench/application.hpp>
#include <bench/gamesettings.hpp>
#include <bench/project.hpp>
#include <bench/windows.h>
#include <editor/gui.hpp>
#include <editor/editor_window.hpp>
#include <stdio.h>
#include "resource.h"

namespace bench_editor {

using namespace bench;

Project* ShowOpenProjectDialog();

int main() {
	InitApplicationOptions options = {};
	options.title = "Bench Editor";
	options.settings_path = "editor_settings.ini";
	InitApplication(options);

	Project* project = ShowOpenProjectDialog();
	if (!project) {
		return 0;
	}

	EditorWindow editor_window;
	editor_window.Init(project);

	while (!ShouldQuit()) {
		BeginFrame();
		EndFrame();
	}
	return 0;

}

}

int
WINAPI
WinMain (
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
    ) {
	bench_editor::main();
	return 0;
}