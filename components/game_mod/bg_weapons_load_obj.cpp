#include "stdafx.h"

// /bgame/bg_weapons_load_obj.cpp:1504
void BG_LoadPlayerAnimTypes()
{
	((void(__cdecl *)())0x00591DB0)();
}

// /bgame/bg_weapons_load_obj.cpp:1574
void BG_ClearWeaponDefInternal()
{
	// surfaceTypeSoundListCount
	*(int *)0x00BE2D3C = 0;
}

// /bgame/bg_weapons_load_obj.cpp:2316
void BG_LoadWeaponMergeSupport()
{
	((void(__cdecl *)())0x00513A70)();
}

// /bgame/bg_weapons_load_obj.cpp:2888
WeaponVariantDef *BG_LoadDefaultWeaponVariantDef()
{
	// Load from rawfiles: BG_LoadDefaultWeaponVariantDef_LoadObj
	if (!useFastFile->current.enabled)
		return ((WeaponVariantDef *(__cdecl *)())0x0076C740)();

	// Load from fastfile: BG_LoadDefaultWeaponVariantDef_FastFile
	return ((WeaponVariantDef *(__cdecl *)())0x0076C7E0)();
}