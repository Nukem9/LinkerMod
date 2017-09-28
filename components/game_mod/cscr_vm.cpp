#include "stdafx.h"

// /clientscript/cscr_vm.cpp:5230
const char *Scr_GetString(unsigned int index, scriptInstance_t inst)
{
	return ((const char *(__cdecl *)(unsigned int, scriptInstance_t))0x00567CB0)(index, inst);
}

// /clientscript/cscr_vm.cpp:5416
void Scr_AddBool(int value, scriptInstance_t inst)
{
	((void(__cdecl *)(int, scriptInstance_t))0x0045DBB0)(value, inst);
}

// /clientscript/cscr_vm.cpp:5603
void Scr_Error(const char *error, bool force_terminal)
{
	((void(__cdecl *)(const char *, bool))0x00644900)(error, force_terminal);
}

// /clientscript/cscr_vm.cpp:5656
void Scr_ObjectError(const char *error, scriptInstance_t inst)
{
	((void(__cdecl *)(const char *, scriptInstance_t))0x0064A840)(error, inst);
}

void SL_RemoveRefToString(scriptInstance_t inst, unsigned int stringValue)
{
	((void(__cdecl *)(scriptInstance_t, unsigned int))0x00545370)(inst, stringValue);
}

void __cdecl GScr_NewDebugHudElem()
{
	if (com_developer->current.enabled)
		GScr_NewHudElem();
}