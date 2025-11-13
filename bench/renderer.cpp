#include <bench/core/window.hpp>
#include <bench/renderer.hpp>
#include <bench/gamesettings.hpp>
#include <d3d9.h>
#include <algorithm>

#pragma comment(lib, "d3d9.lib")

namespace bench {


static IDirect3D9* g_d3d = nullptr;
static IDirect3DDevice9* g_device = nullptr;
static UINT g_adapter = D3DADAPTER_DEFAULT;
static Window g_render_window = {};

static int GetColorDepth(D3DFORMAT format) {
	switch (format) {
		case D3DFMT_A2R10G10B10: return 30;
		case D3DFMT_A8R8G8B8: return 24;
		case D3DFMT_X8R8G8B8: return 24;
		case D3DFMT_A1R5G5B5: return 16;
		case D3DFMT_R5G6B5: return 16;
		case D3DFMT_X1R5G5B5: return 16;
	}
}

static void UpdateSettingsFromDisplayMode(D3DDISPLAYMODE mode) {
	GameSettings().SetInt("window_width", mode.Width);
	GameSettings().SetInt("window_height", mode.Height);
	GameSettings().SetInt("refresh_rate", mode.RefreshRate);
	GameSettings().SetInt("color_depth", GetColorDepth(mode.Format));
	GameSettings().SetInt("fullscreen", 1);
}

D3DDISPLAYMODE GetDisplayModeFromSettings() {
	I32 wanted_width = GameSettings().GetInt("window_width", -1);
	I32 wanted_height = GameSettings().GetInt("window_height", -1);
	I32 color_depth = GameSettings().GetInt("color_depth", -1);
	I32 refresh_rate = GameSettings().GetInt("refresh_rate", -1);
	I32 vsync = GameSettings().GetInt("vsync", 1);

	if (wanted_width < 0 || wanted_height < 0) {
		D3DDISPLAYMODE mode = {};
		g_d3d->GetAdapterDisplayMode(g_adapter, &mode);
		return mode;
	}

	ArrayList<D3DFORMAT> present_formats = {
		D3DFMT_A2R10G10B10,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_A1R5G5B5,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
	};
	std::sort(present_formats.begin(), present_formats.end(), [=](D3DFORMAT a, D3DFORMAT b) {
		auto ScoreFormat = [=](D3DFORMAT format){
			int score = 10000 + format;
			int depth = GetColorDepth(format);
			if (depth == color_depth) score += 10000;
			else score += depth * 100;
			return score;
		};
		return ScoreFormat(a) > ScoreFormat(b);
	});


	int best_score = -1;
	D3DDISPLAYMODE best_mode = {};

	for (D3DFORMAT format : present_formats) {
		for (UINT index = 0;; index++) {
			D3DDISPLAYMODE mode = {};
			HRESULT res = g_d3d->EnumAdapterModes(g_adapter, format, index, &mode);
			if (res != D3D_OK)
				break;

			int score = 10000;

			if (mode.Width == wanted_width) score += 10000;
			else if (wanted_width == -1) score += mode.Width;

			if (mode.Height == wanted_height) score += 10000;
			else if (wanted_height == -1) score += mode.Height;

			if (mode.RefreshRate == refresh_rate) score += 5000;
			else if (refresh_rate == -1) score += mode.RefreshRate;

			if (GetColorDepth(mode.Format) == color_depth) score += 500;
			else if (color_depth == -1) score += color_depth * 50;

			if (score > best_score) {
				best_mode = mode;
				best_score = score;
			}
		}
	}
	return best_mode;
}

void InitRenderer(Window render_window) {
	g_render_window = render_window;

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	AssertAlways(g_d3d, "Failed to init DirectX 9");

	D3DPRESENT_PARAMETERS present_parameters = {};
	present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;

	if (GameSettings().GetInt("fullscreen", 0) == 1) {
		D3DDISPLAYMODE mode = GetDisplayModeFromSettings();
		UpdateSettingsFromDisplayMode(mode);

		present_parameters.BackBufferWidth = mode.Width;
		present_parameters.BackBufferHeight = mode.Height;
		present_parameters.BackBufferFormat = mode.Format;
		present_parameters.BackBufferCount = 2;

		present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
		present_parameters.MultiSampleQuality = 0;

		present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		present_parameters.hDeviceWindow = g_render_window.hwnd;
		present_parameters.Windowed = false;
		present_parameters.EnableAutoDepthStencil = false;
		present_parameters.Flags = 0;

		present_parameters.FullScreen_RefreshRateInHz = mode.RefreshRate;
		present_parameters.PresentationInterval = GameSettings().GetInt("vsync", 1);
	}
	else {
		present_parameters.Windowed = true;
		present_parameters.hDeviceWindow = g_render_window.hwnd;
	}

	g_d3d->CreateDevice(g_adapter, D3DDEVTYPE_HAL, g_render_window.hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present_parameters, &g_device);
	AssertAlways(g_device, "Failed to init DirectX 9");

}

}
