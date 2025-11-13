#pragma once
#include <bench/core/common.hpp>

namespace bench {

struct Window;

struct MeshVertex {
	float px, py, pz;
	float nx, ny, nz;
	float tu, tv;
};

void InitRenderer(Window render_window);

}