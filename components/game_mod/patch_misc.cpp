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
}

#endif
