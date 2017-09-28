#include "stdafx.h"

void Patch_CEG()
{
	PatchMemory(0x00662F20, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Kill Steam DRM IPC creation
	PatchMemory(0x0046C9A0, (PBYTE)"\xCC", 1);
	PatchMemory(0x005F3290, (PBYTE)"\xCC", 1);

	PatchMemory(0x004DFD60, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Registry key checks
	PatchMemory(0x0053F880, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005A5360, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004DFD60, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005A5360, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	PatchMemory(0x0064F6A0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Single function, 32bit hash check
	PatchMemory(0x005614A0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00417360, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0056AB40, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0059BEB0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	PatchMemory(0x00676740, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Direct ExitProcess() check
	PatchMemory(0x005DB020, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004F02C0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00572DF0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00679B40, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004BFB50, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004D4B80, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00501080, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005CAB50, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004C0DE0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	PatchMemory(0x0041CEB0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Wrapper ExitProcess() check, executes
	PatchMemory(0x0041CF50, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// on certain days of the week
	PatchMemory(0x00427E00, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00437350, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00444E80, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00449C30, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004545A0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0045C550, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00462B60, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004682B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00487D80, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0048C7B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004A9970, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004C3360, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004D9650, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x004E3860, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00515B20, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005268E0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00527200, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005474A0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0054F280, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00561410, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0059D820, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005A8800, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005B4330, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005D3F20, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x005EF2A0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x006099E0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00610A60, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00625500, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00625980, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0064FFB0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00662EC0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00662F80, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00671860, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0067B3B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0067D830, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x006857B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0068D300, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x0068E8F0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	PatchMemory(0x009A23B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Wrapper ExitProcess() check with HWBP detection,
	PatchMemory(0x009A23F0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// executes on certain days of the week
	PatchMemory(0x009A2430, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2470, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A24B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A24F0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2530, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2570, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A25B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A25F0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2630, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2670, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A26B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A26F0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2730, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A2770, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	PatchMemory(0x009A2980, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);	// Direct HWBP check
	PatchMemory(0x009A29B0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x009A29E0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	Detours::X86::DetourFunction((PBYTE)0x8EE640, (PBYTE)&sub_8EE640);	// Patch intentional nullptr

	FixupFunction(0x0060CC10, 0x004F20F0);		// CEGObfuscate<LiveStats_Init> => LiveStats_Init
	FixupFunction(0x00580460, 0x0079E6D0);		// CEGObfuscate<Con_Restricted_SetLists> => Con_Restricted_SetLists
}

void *sub_8EE640(void *Nullptr1, void *Nullptr2)
{
	__debugbreak();

	if (Nullptr1 != nullptr || Nullptr2 != nullptr)
		__debugbreak();

	*(void **)0xBA1C24 = Nullptr2;
	return (void *)0xBA1C24;
}