#include <bench/common.hpp>
#include <bench/application.hpp>
#include <bench/utils/defer.hpp>
#include <bench/arena.hpp>
#include <bench/window.hpp>
#include <bench/file.hpp>
#include <bench/coroutine.hpp>
#include <Windows.h>

// undef insane Window.h macros:
#undef CreateWindow
#undef Yield

namespace bench {

struct IOOperation {
	OVERLAPPED overlapped = {};
	CoroutineHandle coro = nullptr;
	I32 num_read = 0;
};

HINSTANCE g_hinstance = nullptr;
static HANDLE g_iocp = nullptr;
static double g_timer_frequency = {};
static LARGE_INTEGER g_timer_start = {};

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_CLOSE: {
			Quit();
			return false;
		}
	}
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}


void InitWin32() {
    g_hinstance = GetModuleHandleA(nullptr);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 2);

	WNDCLASSA wndclass = {};
    wndclass.lpfnWndProc = WindowProc;
    wndclass.hInstance = g_hinstance;

	// NOTE: here we're using LoadCursor the LoadCursor macro, as IDC_ARROW is either LPCWSTR or LPCSTR, based off if UNICODE is defined.
	//       in all other places we use the A/W versions directly.
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW); 
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = "BENCHWNDCLASS";
    RegisterClassA(&wndclass);

	QueryPerformanceCounter(&g_timer_start);

	LARGE_INTEGER timer_frequency_int;
	QueryPerformanceFrequency(&timer_frequency_int);
	g_timer_frequency = double(timer_frequency_int.QuadPart);
}

_When_(!condition, _Analysis_noreturn_)
void AssertAlways(bool condition, const char* fail_message) {
	if (!condition) {
		MessageBoxA(nullptr, fail_message, "Fatal Error", MB_ICONERROR | MB_OK);
		ExitProcess(1);
	}
}

Window CreateWindow(const CreateWindowOptions& options) {
	ScratchArenaView scratch = Arena::Scratch();

    HWND hwnd =::CreateWindowExA(0, "BENCHWNDCLASS", scratch.arena.InternCString(options.title), WS_OVERLAPPEDWINDOW,
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

File File::Open(const char* path, FileFlags flags, FileCreateDisposition create_disposition) {
	DWORD access = 0;
	if (flags & FileFlags::READ) access |= GENERIC_READ;
	if (flags & FileFlags::WRITE) access |= GENERIC_WRITE;

	DWORD attributes = FILE_ATTRIBUTE_NORMAL;
	if (flags & FileFlags::ASYNC) attributes |= FILE_FLAG_OVERLAPPED;

	HANDLE handle = CreateFileA(path, access, 0, nullptr, (DWORD)create_disposition, attributes, nullptr);

	if (flags & FileFlags::ASYNC)
		CreateIoCompletionPort(handle, g_iocp, 0x12345678, 0);

	File file = {};
	file.handle = handle;
	return file;
}

I32 File::Read(I32 size, void* buffer) {
	DWORD num_read = 0;
	BOOL ok = ::ReadFile(this->handle, buffer, size, &num_read, nullptr);
	if (ok)
		return (I32)num_read;
	else
		return -1;
}

I32 File::ReadAsync(Coroutine* coro, I32 size, void* buffer) {
	IOOperation operation = {};
	operation.coro = coro;
	::ReadFile(this->handle, buffer, size, nullptr, &operation.overlapped);
	Yield(coro);
	return operation.num_read;
}

double GetTime() {
	LARGE_INTEGER now = {};
	QueryPerformanceCounter(&now);
	return double(now.QuadPart - g_timer_start.QuadPart) / g_timer_frequency;
}

void File::Seek(I32 offset, FileSeek whence) {
	DWORD method = 0;
	switch (whence) {
		case FileSeek::START: method = FILE_BEGIN; break;
		case FileSeek::CURRENT: method = FILE_CURRENT; break;
		case FileSeek::END: method = FILE_END; break;
	}
	SetFilePointer(this->handle, offset, nullptr, method);
}

U32 File::Tell() {
	DWORD offset = SetFilePointer(this->handle, 0, nullptr, FILE_CURRENT);
	return offset;
}

void File::Close() {
	CloseHandle(this->handle);
	handle = nullptr;
}

I32 File::Write(I32 size, const void* buffer) {
	DWORD num_written = 0;
	BOOL ok = ::WriteFile(this->handle, buffer, size, &num_written, nullptr);
	if (ok) return (I32)num_written;
	else return -1;
}

}