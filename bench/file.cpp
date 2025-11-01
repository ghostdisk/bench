#include <bench/file.hpp>
#include <bench/coroutine.hpp>
#include <windows.h>
#undef Yield

namespace bench {

static HANDLE g_iocp;

struct Operation {
	OVERLAPPED overlapped = {};
	Coroutine* coro = nullptr;
	I32 num_read = 0;
};

void InitFileIO() {
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 2);
}

void PollFileEvents() {
	DWORD bytes = 0;
	ULONG_PTR completion_key = 0;
	OVERLAPPED* overlapped = nullptr;

	for (;;) {
		BOOL val = GetQueuedCompletionStatus(g_iocp, &bytes, &completion_key, &overlapped, 0);
		if (val) {
			Operation* operation = (Operation*)overlapped;
			operation->num_read = bytes;
			ScheduleCoroutine(operation->coro);
		}
		else {
			break;
		}
	}
}

File FileOpen(const char* path) {
	HANDLE handle = CreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);
	HANDLE iocp = CreateIoCompletionPort(handle, g_iocp, 0x12345678, 0);

	File file = {};
	file.handle = handle;
	return file;
}

I32 FileReadAsync(const CoroutineHandle& coro, File& file, I32 size, void* buffer) {

	Operation operation = {};
	operation.coro = coro.state;

	BOOL res = ::ReadFile(file.handle, buffer, size, nullptr, &operation.overlapped);

	Yield(coro);
	return operation.num_read;
}

}
