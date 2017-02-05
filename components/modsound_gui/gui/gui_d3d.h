#pragma once
#include <minidx9\Include\d3d9.h>

#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"

#define D3D_CLEARCOLOR D3DCOLOR_RGBA(14, 57, 102, 1)

enum D3DINSTANCE_STATE
{
	D3D_UNINITIALIZED,
	D3D_INITIALIZED,

	D3D_ERROR,
};

struct d3d_inst_t
{
	LPDIRECT3D9					d3d;
	LPDIRECT3DDEVICE9			device;
	D3DPRESENT_PARAMETERS		present_params;

	D3DINSTANCE_STATE			state;
};

extern d3d_inst_t g_d3d;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int GUI_InitD3D(HWND hWnd);
void GUI_FreeD3D(void);
