#include "stdafx.h"

// 0x00420493
void __declspec(naked) mfh_MapEnts_ParseEntities()
{
	// Drop the node if no custom ents file is present
	_asm
	{
		call MapEnts_CustomEntsFile_IsPresent
		test eax, eax
		jz DROP_NODE

// LOAD_NODE:
		mov eax, 0x00420499
		jmp eax

DROP_NODE:
		mov eax, 0x0042083C
		jmp eax
	}
}