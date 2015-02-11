#include "stdafx.h"

void XModelLoadConfigString(const char **pos, char *buffer)
{
	strcpy(buffer, *pos);

	*pos += strlen(buffer) + 1;
}

SRCLINE(290)
bool XModelLoadConfigFile(const char **pos, int name, int config)
{
	//
	// Check for World at War's version first
	//
	WORD xmodelVersion = *(WORD *)*pos;

	if (xmodelVersion != 25 && xmodelVersion != 62)
	{
		Com_PrintError(19, "ERROR: xmodel '%s' has an bad version (version %d, expecting 25 or 62).\n", name, xmodelVersion);
		return false;
	}

	//
	// Read the configuration header
	//
	// WORD version
	*pos += 2;

	// Black Ops adjustment
	if (xmodelVersion == 62)
		*pos += 3;

	// Flags
	*(BYTE *)(config + 4140) = *(BYTE *)(*pos);
	*pos += 1;

	// Mins
	*(float *)(config + 4112) = *(float *)(*pos);
	*pos += 4;

	*(float *)(config + 4116) = *(float *)(*pos);
	*pos += 4;

	*(float *)(config + 4120) = *(float *)(*pos);
	*pos += 4;

	// Maxs
	*(float *)(config + 4124) = *(float *)(*pos);
	*pos += 4;

	*(float *)(config + 4128) = *(float *)(*pos);
	*pos += 4;

	*(float *)(config + 4132) = *(float *)(*pos);
	*pos += 4;

	// collMapFilename?
	XModelLoadConfigString(pos, (char *)(config + 0x102D));

	// physicsPresetFilename?
	XModelLoadConfigString(pos, (char *)(config + 5165));

	if (xmodelVersion == 62)
	{
		char physicsConstraintsFilename[1024];
		XModelLoadConfigString(pos, physicsConstraintsFilename);

		// Skip a float
		*pos += 4;
	}

	// XModelConfig entries
	float v17;
	const char *v18;
	const char *v19;
	int v20;
	char v21;

	const char *v15 = *pos;
	int v16 = config;

	for (int i = 0; i < 4; i++)
	{
		v17 = *(float *)v15;
		v18 = v15 + 4;
		*(float *)(v16 + 1024) = v17;
		v19 = v18;
		v20 = v16;

		do
		{
			v21 = *v19;
			*(BYTE *)v20++ = *v19++;
		} while (v21);

		v16 += 1028;
		v15 = &v18[strlen(v18) + 1];
	}

	// collLod
	*(DWORD *)(config + 4136) = *(DWORD *)v15;
	*pos = v15 + 4;
		
	return true;
}

bool __declspec(naked) hk_XModelLoadConfigFile()
{
	__asm
	{
		push ebp
		mov ebp, esp

		push [ebp + 0xC]
		push [ebp + 0x8]
		push edi
		call XModelLoadConfigFile
		add esp, 0xC

		pop ebp
		retn
	}
}