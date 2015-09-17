#include "stdafx.h"

void PatchHDR_Lightmaps()
{
	//
	// Remove HDR Clamping when getting lightmap pixels
	//
	PatchMemory(0x00431186, (PBYTE)"\xEB", 1);
	PatchMemory(0x004311AB, (PBYTE)"\xEB", 1);
	PatchMemory(0x004311D0, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431261, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431288, (PBYTE)"\xEB", 1);
	PatchMemory(0x004312AD, (PBYTE)"\xEB", 1);
	PatchMemory(0x0043133F, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431360, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431388, (PBYTE)"\xEB", 1);

	//
	// Remove HDR Pixel Clamping Before Converting to RGB8
	//
	PatchMemory(0x004328AA, (PBYTE)"\xEB", 1);
	PatchMemory(0x004328EC, (PBYTE)"\xEB", 1);
	PatchMemory(0x0043292F, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432986, (PBYTE)"\xEB", 1);
	PatchMemory(0x004329C8, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432A0B, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432A86, (PBYTE)"\xEB", 1);
}