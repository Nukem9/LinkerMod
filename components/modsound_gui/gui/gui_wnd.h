#pragma once
#include <windows.h>

#define WND_TITLE L"MODSound_GUI"
#define WNDCLASS_NAME L"MODSND_GUI_CLS"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct wnd_instance_t
{
	WNDCLASSEX wc;
	HWND hWnd;
};

int GUI_InitWindow(wnd_instance_t* wnd);
void GUI_FreeWindow(wnd_instance_t* wnd);

void GUI_EnterMessageLoop(void);
