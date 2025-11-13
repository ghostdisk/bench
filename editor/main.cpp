#include <bench/core/window.hpp>
#include <bench/core/coroutine.hpp>
#include <bench/application.hpp>
#include <bench/gamesettings.hpp>
#include <editor/gui.hpp>
#include <bench/windows.h>
#include <stdio.h>
#include <CommCtrl.h>
#include "resource.h"
#pragma comment(lib, "Comctl32.lib")

namespace bench_editor {

using namespace bench;

INT_PTR CALLBACK OpenProjectDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
				EndDialog(hDlg, 0);
			return TRUE;
	}
	return FALSE;
}

void MainCoro(CoroutineHandle coro) {
	//GameSettings().GetString("project_path");

}

int main() {
	INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_WIN95_CLASSES };
	InitCommonControlsEx(&icex);

	DialogBoxParamA(GetModuleHandleA(0), MAKEINTRESOURCE(IDD_PROJECT_OPEN_DIALOG), NULL, OpenProjectDialogProc, 0);
	return 0;

	InitApplicationOptions options = {};
	options.title = "Bench Editor";
	InitApplication(options);

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