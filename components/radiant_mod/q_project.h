#pragma once

typedef void (__stdcall* Com_LoadProject_t)(void*);
extern Com_LoadProject_t Com_LoadProject_o;

void __stdcall Com_LoadProject(void* a1);
