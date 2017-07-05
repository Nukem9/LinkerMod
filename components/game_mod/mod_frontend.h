#pragma once

typedef void __cdecl voidFunc();
typedef void __cdecl voidFunc2(int a1, float a2);

typedef int __cdecl UI_UIContext_GetInfo_t(int localClientNum);
static UI_UIContext_GetInfo_t *UI_UIContext_GetInfo = (UI_UIContext_GetInfo_t *)0x4F3C70;

static voidFunc *func_41FEE0 = (voidFunc *)0x41FEE0;

typedef int __cdecl sub_5BEE40_t();
static sub_5BEE40_t *GetLocalClientNum = (sub_5BEE40_t *)0x5BEE40;

// XREFS
// call		at 406B32
// jmp		at 5A6C32
// jmp		at 679018
void Com_RunFrontend();

// XREFS
// replace the Cbuf_AddText call in UI_RunMenuScript
// call		at 45CD7D
void UI_RunMod_HandleFrontend();

void Mod_Frontend_Init();
