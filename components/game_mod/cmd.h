#pragma once

void mfh_Cmd_Exec_f();

extern void(__cdecl * Cmd_ExecuteSingleCommandInternal)(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);
void hk_Cmd_ExecuteSingleCommandInternal(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);
void Cbuf_AddText(int localClientNum, const char *text);