#pragma once

typedef struct HWND__* HWND;
typedef struct HINSTANCE__* HINSTANCE;

namespace bench {

struct Window {
	HWND hwnd = nullptr;
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