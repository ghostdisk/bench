#pragma once
#include <bench/core/string.hpp>

typedef struct HWND__* HWND;
typedef struct HINSTANCE__* HINSTANCE;
typedef struct HMENU__* HMENU;

namespace bench {

struct Window {
	HWND hwnd = nullptr;
};

struct CreateWindowOptions {
	String title = {};
	U32 width = 0;
	U32 height = 0;
	HMENU menu = nullptr;
};

Window CreateWindow(const CreateWindowOptions& options);
void DestroyWindow(Window window);

bool PollWindowEvents();

}