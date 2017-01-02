#pragma once

typedef void __cdecl voidFunc();
typedef void __cdecl voidFunc2(int a1, float a2);

typedef void __cdecl Cbuf_ExecuteBuffer_t(int localClientNum, int controllerIndex, const char* buf);
static Cbuf_ExecuteBuffer_t *Cbuf_ExecuteBuffer = (Cbuf_ExecuteBuffer_t *)0x408A00;

typedef int __cdecl UI_UIContext_GetInfo_t(int a1);
static UI_UIContext_GetInfo_t *UI_UIContext_GetInfo = (UI_UIContext_GetInfo_t *)0x4F3C70;

static voidFunc *func_41FEE0 = (voidFunc *)0x41FEE0;

typedef int __cdecl sub_5BEE40_t();
static sub_5BEE40_t *sub_5BEE40 = (sub_5BEE40_t *)0x5BEE40;

void Com_RunFrontend();