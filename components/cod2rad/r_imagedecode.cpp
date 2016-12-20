#include "stdafx.h"

int __cdecl Image_GetRawPixels(GfxRawImage* image, const char* imageName)
{
	int result = 0;

	_asm
	{
		pushad
		push imageName
		mov esi, image
		mov ebx, 0x00417A50
		call ebx
		add esp, 4
		mov result, eax
		popad
	}

	return result;
}
