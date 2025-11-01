#include <bench/common.hpp>
#include <bench/window.hpp>
#include <bench/file.hpp>
#include <bench/coroutine.hpp>
#include <Windows.h>
#undef CreateWindow

namespace bench {

struct IOOperation {
	OVERLAPPED overlapped = {};
	Coroutine* coro = nullptr;
	I32 num_read = 0;
};

HINSTANCE g_hinstance = nullptr;
static HANDLE g_iocp = nullptr;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}


void InitWin32() {
    g_hinstance = GetModuleHandleA(nullptr);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 2);

	WNDCLASSA wndclass = {};
    wndclass.lpfnWndProc = WindowProc;
    wndclass.hInstance = g_hinstance;
    wndclass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = "BENCHWNDCLASS";
    RegisterClassA(&wndclass);
}

void AssertAlways(bool condition, const char* fail_message) {
	if (!condition) {
		MessageBoxA(nullptr, fail_message, "Fatal Error", MB_ICONERROR | MB_OK);
		ExitProcess(1);
	}
}

Window CreateWindow(const CreateWindowOptions& options) {
    HWND hwnd =::CreateWindowExA(0, "BENCHWNDCLASS", options.title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        options.width, options.height,
        nullptr, // parent,
        nullptr, // menu,
        g_hinstance,
        nullptr); // lparam

    Window window = {};
    window.hwnd = hwnd;

    ShowWindow(hwnd, SW_SHOW);
    return window;
}

void DestroyWindow(Window window) {
    ::DestroyWindow(window.hwnd);
}

bool PollWindowEvents() {
    MSG msg;
	bool had_any = false;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
		had_any = true;
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
	return had_any;
}

bool PollFileEvents() {
	DWORD bytes = 0;
	ULONG_PTR completion_key = 0;
	OVERLAPPED* overlapped = nullptr;

	bool had_any = false;

	for (;;) {
		BOOL val = GetQueuedCompletionStatus(g_iocp, &bytes, &completion_key, &overlapped, 0);
		if (val) {
			had_any = true;
			IOOperation* operation = (IOOperation*)overlapped;
			operation->num_read = bytes;
			ScheduleCoroutine(operation->coro);
		}
		else {
			break;
		}
	}
	return had_any;
}

File FileOpen(const char* path) {
	HANDLE handle = CreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);
	HANDLE iocp = CreateIoCompletionPort(handle, g_iocp, 0x12345678, 0);

	File file = {};
	file.handle = handle;
	return file;
}

I32 FileReadAsync(const CoroutineHandle& coro, File& file, I32 size, void* buffer) {

	IOOperation operation = {};
	operation.coro = coro.state;

	BOOL res = ::ReadFile(file.handle, buffer, size, nullptr, &operation.overlapped);

	Yield(coro);
	return operation.num_read;
}

}