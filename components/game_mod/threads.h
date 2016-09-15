#pragma once

void Sys_ConsoleThread(unsigned int index);
void Sys_CreateThread(void(__cdecl * function)(unsigned int index), unsigned int threadContext);
DWORD WINAPI Sys_ThreadMain(LPVOID Arg);