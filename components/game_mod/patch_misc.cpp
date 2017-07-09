#include "stdafx.h"
#if USE_MISC_PATCHES

//
// Useful for various debug tweaks
//
void PatchMisc()
{
	//
	// Disable "cannot spawn turret '%s' which has a script, unless one instance placed in map"
	// Allows devmapping into creek_1 with g_loadScripts 0
	//
	PatchMemory_WithNOP(0x004D5061, 5);

	//
	// Graphics
	//

	//
	// Force all fullscreen rendertargets to use full-res mips
	//

	// We have 4 bytes to work with here
	// We need to replace the following instructions
	// To set ebx to 0 (which removes the need for the shifts)
	//	EBX *must* be set to zero so that a picmip value of 0 is
	//	passed to Image_TrackFullscreenTexture() later in the function
	/*
	mov ecx, ebx
	sar eax, cl			// No longer needed
	*/
	//PatchMemory(0x0071F1E8, (PBYTE)
	//	"\x31\xDB" // xor ebx, ebx : set ebx (picmip) to 0
	//	"\x89\xD9", // mov ecx, ebx
	//	4);
	//
	//// Overwrite the second shift just to be safe
	//PatchMemory_WithNOP(0x0071F1FB, 2);
	//
	////
	//// Change Composite Rendertarget Res
	////
	//const int rt_composite_res = 1024; // 256
	//PatchMemory(0x0071F277, (PBYTE)&rt_composite_res, 4);
	//PatchMemory(0x0071F27C, (PBYTE)&rt_composite_res, 4);
	//PatchMemory(0x0071F2A2, (PBYTE)&rt_composite_res, 4);
	
}

#endif
