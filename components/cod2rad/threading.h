#pragma once

DWORD WINAPI ForEachQuantumWorkerThread(LPVOID ThreadParameter);
void ForEachQuantumMultiThreaded(ULONG ThreadCount, void(__cdecl *Callback)(int, int));

void PatchThreading();