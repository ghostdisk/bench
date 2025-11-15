#include <bench/windows.h>
#include <CommCtrl.h>
#include <editor/editor_window.hpp>

namespace bench_editor {

void EditorWindow::Init(Project* project) {
	assert(project);
	m_project = project;

	CreateWindowOptions window_options = {};
	window_options.title = "Bench SDK Editor";
	window_options.width = 1280;
	window_options.height = 720;
	m_window = CreateWindow(window_options);

	HWND toolbar = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS |TBSTYLE_FLAT | CCS_TOP | BTNS_AUTOSIZE,
		0, 0, 500, 50,
		m_window.hwnd, nullptr, g_hinstance, nullptr);
	SendMessageW(toolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);
	SendMessageW(toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessageW(toolbar, TB_AUTOSIZE, 0, 0);
	//SendMessageW(toolbar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_HIDECLIPPEDBUTTONS);

	TBBUTTON button = {};
	button.iBitmap = STD_FILENEW;
	button.fsState = TBSTATE_ENABLED;
    button.fsStyle = TBSTYLE_BUTTON;
    button.idCommand = 1000;

	SendMessage(toolbar, TB_SETBUTTONSIZE, (WPARAM)0, (LPARAM)MAKELONG(16, 16));
	SendMessage(toolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);
	SendMessage(toolbar, TB_AUTOSIZE, 0, 0);

	/*
	m_rebar = CreateWindowExW(
		WS_EX_TOOLWINDOW,
		REBARCLASSNAMEW,
		nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_VARHEIGHT | CCS_NODIVIDER,
		0, 0, 0, 0,
		m_window.hwnd,
		NULL,
		g_hinstance,
		NULL);
	REBARINFO rebar_info = {};
	rebar_info.cbSize = sizeof(rebar_info);
	SendMessageW(m_rebar, RB_SETBARINFO, 0, (LPARAM)&rebar_info);

	REBARBANDINFOW band_info = {};
	band_info.cbSize = sizeof(band_info);
	band_info.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_SIZE;
	band_info.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP;
	band_info.hwndChild = combo_box;
	band_info.lpText = L"COMBO BOXX";
	band_info.cx = 500;  // length
	SendMessageW(m_rebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&band_info);
	*/

}

}