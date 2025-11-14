#pragma once
#include <bench/windows.h>
#include <bench/core/string.hpp>
#include <bench/core/arena.hpp>
#include <bench/gamesettings.hpp>
#include "../resource.h"

namespace bench_editor {

using namespace bench;

class OpenProjectDialog {
public:
	HeapString m_project_path = {};
	HWND m_window_handle = nullptr;

	void UpdateProjectPathFromEditText() {
		ScratchArenaView scratch = Arena::Scratch();
		wchar_t wide_buffer[MAX_PATH];
		GetDlgItemTextW(m_window_handle, IDC_EDIT_PATH, wide_buffer, MAX_PATH);
		m_project_path = scratch.arena.InternString(wide_buffer);

		GameSettings().SetString("saved_project_path", m_project_path);
		GameSettings().Save();
	}

	void SetProjectPath(String path) {
		ScratchArenaView scratch = Arena::Scratch();
		SetDlgItemTextW(m_window_handle, IDC_EDIT_PATH, scratch.arena.InternWideCString(path));
	}

	INT_PTR CALLBACK HandleMessage(UINT msg, WPARAM wparam, LPARAM lparam) {
		ScratchArenaView scratch = Arena::Scratch();

		switch (msg) {
			case WM_COMMAND: {
				int control_id = LOWORD(wparam);
				int command_code = HIWORD(wparam);

				switch (control_id) {
					case IDC_BUTTON_BROWSE: {

						wchar_t path_buffer[MAX_PATH];
						path_buffer[0] = '\0';

						OPENFILENAMEW ofn = {};
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = m_window_handle;
						ofn.lpstrFilter = L"Bench SDK Project (*.bsprj)\0*.bsprj\0" "All Files\0*.*\0";
						ofn.lpstrFile = path_buffer;
						ofn.nMaxFile = sizeof(path_buffer);
						ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

						if (GetOpenFileNameW(&ofn)) {
							SetProjectPath(scratch.arena.InternString(path_buffer));
						}
						break;
					}
					case IDC_EDIT_PATH: {
						if (command_code == EN_CHANGE) {
							UpdateProjectPathFromEditText();
						}
						break;
					}
					case IDC_BUTTON_NEWPROJECT: {
						break;
					}
				}
				return true;
			}
			case WM_CLOSE: {
				EndDialog(m_window_handle, 1);
				return true;
			}
			default: {
				return false;
			}
		}
	}
};

static INT_PTR CALLBACK OpenProjectDialogProc(HWND window_handle, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_INITDIALOG: {
			ScratchArenaView scratch = Arena::Scratch();

			OpenProjectDialog* dialog = (OpenProjectDialog*)lparam;
			SetWindowLongW(window_handle, GWLP_USERDATA, (LONG_PTR)dialog);
			dialog->m_window_handle = window_handle;
			SetDlgItemTextW(window_handle, IDC_EDIT_PATH, scratch.arena.InternWideCString(dialog->m_project_path));
			return true;
		}
		default: {
			OpenProjectDialog* dialog = (OpenProjectDialog*)GetWindowLongW(window_handle, GWLP_USERDATA);
			return dialog->HandleMessage(msg, wparam, lparam);
		}
	}
	return false;
}

void ShowOpenProjectDialog() {
	OpenProjectDialog dialog = {};
	dialog.m_project_path = GameSettings().GetString("saved_project_path");

	INT_PTR dialog_result = DialogBoxParamW(g_hinstance, MAKEINTRESOURCEW(IDD_PROJECT_OPEN_DIALOG), NULL, OpenProjectDialogProc, (LPARAM)&dialog);
	if (dialog_result <= 0) {
		MessageBoxA(nullptr, "Failed to open the Bench SDK Project Selection Wizard", "Error", MB_ICONERROR | MB_OK);
	}
}

}