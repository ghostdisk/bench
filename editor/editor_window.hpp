#pragma once
#include <bench/core/window.hpp>
#include <bench/project.hpp>

namespace bench_editor {

using namespace bench;

class EditorWindow {
public:
	Window m_window = {};
	Project* m_project = nullptr;
	HWND m_rebar = nullptr;

	void Init(Project* project);
};

}