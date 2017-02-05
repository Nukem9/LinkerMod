#include "gui_d3d.h"
#include "../common/io.h"

#pragma comment(lib, "../shared/minidx9/Lib/x86/d3d9.lib")
#pragma comment(lib, "imgui.lib")

d3d_inst_t g_d3d;

int GUI_InitD3D(HWND hWnd)
{
	switch (g_d3d.state)
	{
	case D3D_UNINITIALIZED:
		// Continue initializing
		break;
	case D3D_INITIALIZED:
		Con_Warning("Warning: Attempted to initialize d3d more when it's already been initialized\n");
		return 0;
	default:
		Con_Error("Error: Attempted to init d3d while in unsupported state (0x%X)\n", g_d3d.state);
		return 1;
	}

	g_d3d.d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_d3d.d3d == NULL)
	{
		ZeroMemory(&g_d3d, sizeof(g_d3d));
		return 2;
	}

	ZeroMemory(&g_d3d.present_params, sizeof(g_d3d.present_params));
	g_d3d.present_params.Windowed = TRUE;
	g_d3d.present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3d.present_params.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3d.present_params.EnableAutoDepthStencil = TRUE;
	g_d3d.present_params.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3d.present_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	int err = g_d3d.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3d.present_params, &g_d3d.device);
	if (err < 0)
	{
		g_d3d.d3d->Release();
		return 3;
	}

	return 0;
}

void GUI_FreeD3D(void)
{
	if (g_d3d.state == D3D_INITIALIZED)
	{
		if (g_d3d.device)
			g_d3d.device->Release();

		if (g_d3d.d3d)
			g_d3d.d3d->Release();
	}
	ZeroMemory(&g_d3d, sizeof(g_d3d));
}
