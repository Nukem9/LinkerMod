#include "stdafx.h"

#define LIGHTDEF_DEFAULT "light_dynamic"

void GetDegammadImage(GfxRawImage* image, GfxLightImage* lightImage)
{
	_asm
	{
		pushad
		push lightImage
		mov edi, image
		mov ebx, 0x0042AAE0
		call ebx
		add esp, 4
		popad
	}
}

GfxLightDef *__cdecl RegisterLightDef(char *lightDefName)
{
	for (int i = 0; i < pointLightGlob.defCount; i++)
	{
		GfxLightDef* def = &pointLightGlob.lightDefs[i];
		if (_stricmp(def->name, lightDefName) == 0)
		{
			return def;
		}
	}

	if (pointLightGlob.defCount >= 64)
	{
		Com_FatalError("More than %i lightDefs used by all lights combined; can't load '%s'\n", 64, lightDefName);
		return NULL;
	}

	GfxLightDef* def = &pointLightGlob.lightDefs[pointLightGlob.defCount];

	char lightDefPath[MAX_PATH];
	sprintf_s(lightDefPath, "lights/%s", lightDefName);

	const char* file = NULL;
	int fileSize = FS_ReadFile(lightDefPath, (void**)&file);

	// A lightdef file needs both the samplerState byte plus a null terminated string for the def image name (meaning it must be >= 2 bytes)
	if (fileSize < 3)
	{
		Com_FatalError("Couldn't get light def images for '%s'\n", lightDefName);
		return NULL;
	}

	GfxRawImage imageHeader;
	if (strlen(file + 1) == 0)
	{
		memset(&imageHeader, 0, sizeof(GfxRawImage));
		FS_FreeFile((void*)file);
	}
	else
	{
		Image_GetRawPixels(&imageHeader, file + 1);
		FS_FreeFile((void*)file);
	}

	if (imageHeader.height != 1)
	{
		if (_stricmp(lightDefName, LIGHTDEF_DEFAULT) == 0)
		{
			Com_FatalError("Falloff image %s in light def %s has dimensions %ix%i; height should be 1\n", imageHeader.name, lightDefName, imageHeader.width, imageHeader.height);
			return NULL;
		}
		
		printf("Falloff image %s in light def %s has dimensions %ix%i; height should be 1; Overriding with default light def %s",
			imageHeader.name, lightDefName, imageHeader.width, imageHeader.height, LIGHTDEF_DEFAULT);
		return RegisterLightDef(LIGHTDEF_DEFAULT);
	}

	def->name = _strdup(lightDefName);
	GetDegammadImage(&imageHeader, &def->attenuation);
	
	++pointLightGlob.defCount;
	printf("Registered lightDef '%s'\n", lightDefName);

	return def;
}
