#include <bench/core/window.hpp>
#include <bench/core/coroutine.hpp>
#include <bench/application.hpp>
#include <bench/gamesettings.hpp>
#include <bench/project.hpp>
#include <editor/gui.hpp>
#include <bench/windows.h>
#include <stdio.h>
#include "resource.h"

namespace bench_editor {

using namespace bench;

Project* ShowOpenProjectDialog();

void MainCoro(CoroutineHandle coro) {
	//GameSettings().GetString("project_path");
}

int main() {
	InitApplicationOptions options = {};
	options.title = "Bench Editor";
	options.settings_path = "editor_settings.ini";
	InitApplication(options);

	Project* project = ShowOpenProjectDialog();
	if (!project) {
		return 0;
	}
	else {
		MessageBoxA(0, "Yes project", "Msg", MB_OK);
	}

	return 0;
	StartCoroutine([](CoroutineHandle coro) {
		MainCoro(coro);
	});

	/*
	HMENU main_menu = CreateMenu();
	AppendMenuA(main_menu, 0, 1, "File");
	AppendMenuA(main_menu, 0, 1, "Edit");

	CreateWindowOptions main_window_options = {};
	main_window_options.title = options.title;
	main_window_options.width = GameSettings().GetInt("window_width", 1280);
	main_window_options.height = GameSettings().GetInt("window_height", 720);
	main_window_options.menu = main_menu;
	Window main_window = CreateWindow(main_window_options);

	HWND button = CreateWindowA(
		"BUTTON", "Click", BS_FLAT | BS_PUSHBUTTON | WS_CHILD, 8, 8, 200, 30, main_window.hwnd, nullptr, GetModuleHandleA(0), nullptr);
	ShowWindow(button, SW_SHOW);
	*/

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