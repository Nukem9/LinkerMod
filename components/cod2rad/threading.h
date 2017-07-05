#pragma once

typedef void(__cdecl* QuantumFunc_t)(int stepIndex, int a2);

void __cdecl ForEachQuantum(int stepCount, void(__cdecl *func)(int, int), int threadCount);
DWORD WINAPI ForEachQuantumWorkerThread(LPVOID ThreadParameter);
void ForEachQuantumMultiThreaded(ULONG ThreadCount, void(__cdecl *Callback)(int, int));

void PatchThreading();