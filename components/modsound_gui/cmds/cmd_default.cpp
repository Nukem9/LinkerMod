#include "cmd_common.h"
#include "../cmd.h"
#include "../arg.h"

#include "../gui/gui_d3d.h"
#include "../gui/gui_wnd.h"

int Cmd_Default_f(int argc, char** argv)
{
	wnd_instance_t wnd;
	GUI_InitWindow(&wnd);

	int err = GUI_InitD3D(wnd.hWnd);
	if (err != 0)
	{
		Con_Error("Fatal Error: Unable to initialize D3D\n");
		GUI_FreeWindow(&wnd);
		return 1;
	}

	ImGui_ImplDX9_Init(wnd.hWnd, g_d3d.device);
	GUI_EnterMessageLoop(&wnd);
	ImGui_ImplDX9_Shutdown();

	GUI_FreeD3D();
	GUI_FreeWindow(&wnd);
	return 0;
}