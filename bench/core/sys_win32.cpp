#include <bench/core/common.hpp>
#include <bench/core/arena.hpp>
#include <bench/core/file.hpp>
#include <bench/core/sys.hpp>
#include <bench/core/window.hpp>
#include <bench/core/coroutine.hpp>
#include <bench/application.hpp>
#include <bench/windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

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

	File::StdIn.handle = GetStdHandle(STD_INPUT_HANDLE);
	File::StdOut.handle = GetStdHandle(STD_OUTPUT_HANDLE);
	File::StdErr.handle = GetStdHandle(STD_ERROR_HANDLE);

	INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_WIN95_CLASSES };
	InitCommonControlsEx(&icex);
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
        options.menu, // menu,
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
			UnblockCoroutine(operation->coro, 1);
		}
		else {
			break;
		}
	}
	return had_any;
}

File File::Open(String path, FileFlags flags, FileCreateDisposition create_disposition) {
	ScratchArenaView scratch = Arena::Scratch();

	DWORD access = 0;
	if (flags & FileFlags::READ) access |= GENERIC_READ;
	if (flags & FileFlags::WRITE) access |= GENERIC_WRITE;

	DWORD attributes = FILE_ATTRIBUTE_NORMAL;
	if (flags & FileFlags::ASYNC) attributes |= FILE_FLAG_OVERLAPPED;

	wchar_t* wide_path = scratch.arena.InternWideCString(path);
	HANDLE handle = CreateFileW(wide_path, access, 0, nullptr, (DWORD)create_disposition, attributes, nullptr);

	if (handle != INVALID_HANDLE_VALUE) {
		if (flags & FileFlags::ASYNC)
			CreateIoCompletionPort(handle, g_iocp, 0x12345678, 0);

		File file = {};
		file.handle = handle;
		return file;
	}
	else {
		auto err = GetLastError();
		File file = {};
		file.handle = INVALID_HANDLE_VALUE;
		return file;
	}


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
	BlockCoroutine(coro, 1);
	operation.coro = coro;
	DWORD num_read = 0;
	BOOL was_sync_read = ::ReadFile(this->handle, buffer, size, &num_read, &operation.overlapped);
	if (was_sync_read) {
		UnblockCurrentCoroutine(coro, 1);
		return (I32)num_read;
	}
	else {
		Yield(coro);
		return operation.num_read;
	}
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

static U32 TranslateVirtualMemoryProtection(VirtualMemoryProtection protection) {
	switch (protection.value) {
		case VirtualMemoryProtection::READ: {
			return PAGE_READONLY;
		}
		case VirtualMemoryProtection::READ | VirtualMemoryProtection::WRITE: {
			return PAGE_READWRITE;
		}
		case VirtualMemoryProtection::READ | VirtualMemoryProtection::EXECUTE: {
			return PAGE_EXECUTE_READ;
		}
		case VirtualMemoryProtection::READ | VirtualMemoryProtection::WRITE | VirtualMemoryProtection::EXECUTE: {
			return PAGE_EXECUTE_READWRITE;
		}
		default: {
			return PAGE_NOACCESS;
		}
	}
}

void* VirtualAlloc(void* address, U32 size, VirtualAllocType type, VirtualMemoryProtection protection) {
	U32 win32_protection = TranslateVirtualMemoryProtection(protection);

	U32 win32_type = 0;
	switch (type) {
		case VirtualAllocType::RESERVE: win32_type = MEM_RESERVE; break;
		case VirtualAllocType::COMMIT: win32_type = MEM_COMMIT; break;
	}

	return ::VirtualAlloc(address, size, win32_type, win32_protection);
}

void VirtualFree(void* address, U32 size, VirtualFreeType type) {
	U32 win32_type = 0;
	switch (type) {
		case VirtualFreeType::DECOMMIT: win32_type = MEM_DECOMMIT; break;
		case VirtualFreeType::RELEASE: win32_type = MEM_RELEASE; break;
	}
	::VirtualFree(address, 0, win32_type);
}

String GetCurrentDirectory(Arena& arena) {
	DWORD buffer_size = ::GetCurrentDirectoryW(0, nullptr);
	if (buffer_size == 0) {
		return String();
	}

	wchar_t* wide_buffer = (wchar_t*)arena.Allocate(buffer_size * 2, alignof(wchar_t));
	GetCurrentDirectoryW(buffer_size, wide_buffer);
	return arena.InternString(wide_buffer);

}

String GetAbsolutePath(Arena& arena, String path) {
	wchar_t* wide_path = arena.InternWideCString(path);
	DWORD wide_absolute_path_size = GetFullPathNameW(wide_path, 0, nullptr, nullptr);
	if (wide_absolute_path_size == 0) {
		return {};
	}

	wchar_t* wide_absolute_path = (wchar_t*)arena.Allocate(wide_absolute_path_size * sizeof(wchar_t), alignof(wchar_t));
	GetFullPathNameW(wide_path, wide_absolute_path_size, wide_absolute_path, nullptr);

	return arena.InternString(wide_absolute_path);
}

}