#include "cmd_common.h"
#include "../cmd.h"
#include "../arg.h"

#include "../gui/gui_d3d.h"
#include "../gui/gui_wnd.h"

#include "../csv/csv.h"
#include "../csv/csv_metadata.h"

int Cmd_Default_f(int argc, char** argv)
{
	CSV_Metadata_Init();

	Con_Printf("Create window...\n");
	wnd_instance_t wnd;
	GUI_InitWindow(&wnd);

	Con_Printf("Init Direct3D...\n");
	int err = GUI_InitD3D(wnd.hWnd);
	if (err != 0)
	{
		Con_Error("Fatal Error: Unable to initialize D3D\n");
		GUI_FreeWindow(&wnd);
		return 1;
	}

	Con_Printf("Init ImGui...\n");
	ImGui_ImplDX9_Init(wnd.hWnd, g_d3d.device);

	//
	// Setup the default font
	//
	ImGuiIO& io = ImGui::GetIO();

	//consola.ttf
	//segoeui.ttf
	//io.Fonts->AddFontFromFileTTF("C:/Windows/fonts/segoeui.ttf", 20.0f);

	Con_Printf("Entering message loop...\n");
	GUI_EnterMessageLoop(&wnd);
	
	Con_Printf("Free ImGui...\n");
	ImGui_ImplDX9_Shutdown();

	Con_Printf("Free Direct3D...\n");
	GUI_FreeD3D();

	Con_Printf("Free window...\n");
	GUI_FreeWindow(&wnd);

	CSV_Metadata_Free();
	return 0;
}