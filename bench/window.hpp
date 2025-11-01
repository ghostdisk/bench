#pragma once

#ifdef BENCH_WIN32
typedef struct HWND__* HWND;
typedef struct HINSTANCE__* HINSTANCE;
#endif

namespace bench {

struct Window {
#ifdef BENCH_WIN32
	HWND hwnd = nullptr;
#endif
};

struct CreateWindowOptions {
	const char* title;
	U32 width;
	U32 height;
};

Window CreateWindow(const CreateWindowOptions& options);
void DestroyWindow(Window window);

bool PollWindowEvents();

}