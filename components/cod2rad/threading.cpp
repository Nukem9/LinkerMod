#include "stdafx.h"

const static int MAX_THREADS = 32;

const static int MAX_MAP_DRAW_VERTS = 0x90000;
const static int MAX_MAP_TRIANGLES = 0x60000;

//
// Reallocated structures
//
BYTE	g_GeoGlob[12 * MAX_THREADS];
BYTE	g_TrisData[36 * MAX_MAP_TRIANGLES];
BYTE	g_BlockAllocData[2 * 36 * MAX_THREADS];
DWORD	g_SuppressedLightingSamples[MAX_THREADS];
DWORD	g_RadiosityBounce[MAX_THREADS];

//
// Internal thread callbacks
//
void(__cdecl *g_QuantumWorkerCallback)(int, int);
volatile DWORD *g_ThreadLocks = (DWORD *)0x17302434;
volatile DWORD g_ThreadCounter;

void __cdecl ForEachQuantum(int stepCount, QuantumFunc_t func, int threadCount)
{
	*(LONG *)0x17303430 = stepCount;
	SetProgress(0, stepCount);
	if (threadCount == 1)
		for (int i = 0; i < *(LONG *)0x17303430; i++)
		{
			func(i, 0);
			UpdateProgress(1);
		}
	else
		ForEachQuantumMultiThreaded(threadCount, func);
}

DWORD WINAPI ForEachQuantumWorkerThread(LPVOID ThreadParameter)
{
	for (LONG i = InterlockedExchangeAdd(&g_ThreadCounter, 1); i < *(LONG *)0x17303430; i = InterlockedExchangeAdd(&g_ThreadCounter, 1))
	{
		g_QuantumWorkerCallback(i, (int)ThreadParameter);

		if (!ThreadParameter)
			SetProgress(i + 1, *(LONG *)0x17303430);
	}

	return 0;
}

void ForEachQuantumMultiThreaded(ULONG ThreadCount, void(__cdecl *Callback)(int, int))
{
	printf("Threads: %d\n", ThreadCount);

	//
	// Thread locking structure (reset)
	//
	memset((PVOID)g_ThreadLocks, 0, sizeof(LONG) * 1021);
	g_ThreadCounter = 0;

	//
	// Allocate handles
	//
	HANDLE *handles = new HANDLE[ThreadCount];
	ULONG *threadIds = new ULONG[ThreadCount];

	//
	// Create each thread
	//
	g_QuantumWorkerCallback = Callback;

	for (ULONG i = 0; i < ThreadCount; i++)
		handles[i] = CreateThread(nullptr, 0, ForEachQuantumWorkerThread, (LPVOID)i, 0, &threadIds[i]);

	//
	// Wait for each instance to complete
	//
	WaitForMultipleObjects(ThreadCount, handles, TRUE, INFINITE);

	//
	// Free all resources
	//
	for (ULONG i = 0; i < ThreadCount; i++)
		CloseHandle(handles[i]);

	delete[] handles;
	delete[] threadIds;
}

void __declspec(naked) hk_ForEachQuantumMultiThreaded()
{
	__asm
	{
		push dword ptr ss:[esp + 4]
		push edi
		call ForEachQuantumMultiThreaded
		add esp, 0x8
		retn
	}
}

void __declspec(naked) hk_SuppressedLightingInit()
{
	memset(&g_SuppressedLightingSamples, 0xFF, sizeof(g_SuppressedLightingSamples));

	*(DWORD *)0x16E99F88 = 0;

	__asm retn
}

void __declspec(naked) hk_RadiosityInit()
{
	__asm pushad

	memset(&g_RadiosityBounce, 0, sizeof(g_RadiosityBounce));

	__asm
	{
		popad

		mov eax, 0x0043EBD8
		jmp eax
	}
}

void FixupGeoHunk()
{
	// geoGlob.hunk[threadIndex]
	REMAP(0x0043CBD3, 0x16E99368, (ULONG_PTR)&g_GeoGlob);

	REMAP(0x00437FB6, 0x16E9936C, (ULONG_PTR)&g_GeoGlob + (0x16E9936C - 0x16E99368));
	REMAP(0x0043CBC1, 0x16E9936C, (ULONG_PTR)&g_GeoGlob + (0x16E9936C - 0x16E99368));

	REMAP(0x00437F33, 0x16E99370, (ULONG_PTR)&g_GeoGlob + (0x16E99370 - 0x16E99368));
	REMAP(0x0043CBCC, 0x16E99370, (ULONG_PTR)&g_GeoGlob + (0x16E99370 - 0x16E99368));

	//PageGuard_Monitor(0x16E99368, 4 * 12);
}

void FixupGlobalTris()
{
	// Tris
	REMAP(0x00439EE7, 0x15999354, (ULONG_PTR)&g_TrisData);
	REMAP(0x0043A65F, 0x15999354, (ULONG_PTR)&g_TrisData);
	REMAP(0x0043B482, 0x15999354, (ULONG_PTR)&g_TrisData);
	REMAP(0x0043C12C, 0x15999354, (ULONG_PTR)&g_TrisData);
	REMAP(0x0043C57D, 0x15999354, (ULONG_PTR)&g_TrisData);
	REMAP(0x0043C5FD, 0x15999354, (ULONG_PTR)&g_TrisData);
	REMAP(0x00448B6F, 0x15999354, (ULONG_PTR)&g_TrisData);

	REMAP(0x0043C3FF, 0x15999370, (ULONG_PTR)&g_TrisData + (0x15999370 - 0x15999354));

	REMAP(0x0043D52F, 0x15999372, (ULONG_PTR)&g_TrisData + (0x15999372 - 0x15999354));

	REMAP(0x0043A657, 0x15999373, (ULONG_PTR)&g_TrisData + (0x15999373 - 0x15999354));
	REMAP(0x0043B47A, 0x15999373, (ULONG_PTR)&g_TrisData + (0x15999373 - 0x15999354));
	REMAP(0x0043C124, 0x15999373, (ULONG_PTR)&g_TrisData + (0x15999373 - 0x15999354));

	//PageGuard_Monitor(0x15999354, 36 * 4);
}

void FixupBlockAlloc()
{
	// BlockAlloc structure
	REMAP(0x0043E529, 0x153C9224, (ULONG_PTR)&g_BlockAllocData);
	REMAP(0x0043E68E, 0x153C9224, (ULONG_PTR)&g_BlockAllocData);
	REMAP(0x0043E6E7, 0x153C92B4, (ULONG_PTR)&g_BlockAllocData + sizeof(g_BlockAllocData) / 2);
	REMAP(0x0043E540, 0x00000090, sizeof(g_BlockAllocData) / 2);

	//PageGuard_Monitor(0x153C9224, 4 * 36);
	//PageGuard_Monitor(0x153C92B4, 4 * 36);
}

void FixupSuppressedLightingSamples()
{
	// suppressedLightingSamples[threadIndex]
	REMAP(0x0042F3F8, 0x16E99F78, (ULONG_PTR)&g_SuppressedLightingSamples);
	REMAP(0x0042F471, 0x16E99F78, (ULONG_PTR)&g_SuppressedLightingSamples);
	REMAP(0x0042FF78, 0x16E99F78, (ULONG_PTR)&g_SuppressedLightingSamples);

	Detours::X86::DetourFunction((PBYTE)0x0042FF78, (PBYTE)&hk_SuppressedLightingInit);

	//PageGuard_Monitor(0x16E99F78, 4 * 4);
}

void FixupRadiosityBounce()
{
	// radiosityBounce[threadIndex]
	REMAP(0x0043D835, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043D84C, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043EBC4, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043EC50, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043EC5A, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043EC6E, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043EC80, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043EC94, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ECA6, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ECBA, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ECCC, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ECE0, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ED00, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ED0A, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ED1E, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);
	REMAP(0x0043ED2D, 0x153C920C, (ULONG_PTR)&g_RadiosityBounce);

	REMAP(0x0043EBC9, 0x153C9210, (ULONG_PTR)&g_RadiosityBounce + (0x153C9210 - 0x153C920C));
	REMAP(0x0043EC76, 0x153C9210, (ULONG_PTR)&g_RadiosityBounce + (0x153C9210 - 0x153C920C));

	REMAP(0x0043EBCE, 0x153C9214, (ULONG_PTR)&g_RadiosityBounce + (0x153C9214 - 0x153C920C));
	REMAP(0x0043EC9C, 0x153C9214, (ULONG_PTR)&g_RadiosityBounce + (0x153C9214 - 0x153C920C));

	REMAP(0x0043EBD3, 0x153C9218, (ULONG_PTR)&g_RadiosityBounce + (0x153C9218 - 0x153C920C));
	REMAP(0x0043ECC2, 0x153C9218, (ULONG_PTR)&g_RadiosityBounce + (0x153C9218 - 0x153C920C));

	Detours::X86::DetourFunction((PBYTE)0x0043EBC4, (PBYTE)&hk_RadiosityInit);

	//PageGuard_Monitor(0x153C920C, 4 * 4);
}

void PatchThreading()
{
	PatchMemory(0x00440BDB, (PBYTE)&MAX_THREADS, 1);// PCL_Threads
	PatchMemory(0x00440A50, (PBYTE)&MAX_THREADS, 1);// SetDefaultOptions
	PatchMemory(0x00440A54, (PBYTE)&MAX_THREADS, 1);// SetDefaultOptions

	Detours::X86::DetourFunction((PBYTE)0x004291D0, (PBYTE)&hk_ForEachQuantumMultiThreaded);

	FixupGeoHunk();
	FixupGlobalTris();
	FixupBlockAlloc();
	FixupSuppressedLightingSamples();
	FixupRadiosityBounce();
}
