#include "stdafx.h"

Com_LoadProject_t Com_LoadProject_o = NULL;

void __stdcall Com_LoadProject(void* a1)
{
	Com_LoadProject_o(a1);
	App_DestroySplash();
}
