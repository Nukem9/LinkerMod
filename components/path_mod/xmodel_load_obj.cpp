#include "stdafx.h"

#define XMODEL_READ_BYTE(dest)		*(BYTE *)(dest) = *(BYTE *)(*pos); *pos += sizeof(BYTE);
#define XMODEL_READ_FLOAT(dest)		*(float *)(dest) = *(float *)(*pos); *pos += sizeof(float);
#define XMODEL_READ_STRING(dest)	XModelLoadConfigString(pos, (char *)(dest));

void XModelLoadConfigString(const char **pos, char *buffer)
{
	strcpy_s(buffer, 1024, *pos);

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
	*pos += 2;

	// Black Ops adjustment
	if (xmodelVersion == 62)
		*pos += 3;

	// Flags
	XMODEL_READ_BYTE(config + 4140);

	// Mins
	XMODEL_READ_FLOAT(config + 4112);
	XMODEL_READ_FLOAT(config + 4116);
	XMODEL_READ_FLOAT(config + 4120);

	// Maxs
	XMODEL_READ_FLOAT(config + 4112);
	XMODEL_READ_FLOAT(config + 4128);
	XMODEL_READ_FLOAT(config + 4132);

	// collMapFilename?
	XMODEL_READ_STRING(config + 0x102D);

	// physicsPresetFilename?
	XMODEL_READ_STRING(config + 0x142D);

	if (xmodelVersion == 62)
	{
		char physicsConstraintsFilename[1024];
		XMODEL_READ_STRING(physicsConstraintsFilename);

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

		push edi //config
		push [ebp + 0x8] //name
		push esi //pos
		call XModelLoadConfigFile
		add esp, 0xC

		pop ebp
		retn
	}
}