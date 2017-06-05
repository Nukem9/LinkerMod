#include "stdafx.h"

VANILLA_VALUE(font, struct Font_s*, 0x02FF55F8);

col_context_t::col_context_t(void)
{
	this->mask = 0;
	this->prims = 0;
	this->nprims = 0;
	this->ignoreEntParams = 0;
	this->passEntityNum0 = 1023;
	this->passEntityNum1 = 1023;
	this->locational = 0;
	this->staticmodels = 0;
	this->priorityMap = 0;
	this->collide_entity_func = 0;
}

col_context_t::col_context_t(int _mask)
{
	this->mask = _mask;
	this->prims = 0;
	this->nprims = 0;
	this->ignoreEntParams = 0;
	this->passEntityNum0 = 1023;
	this->passEntityNum1 = 1023;
	this->locational = 0;
	this->staticmodels = 0;
	this->priorityMap = 0;
	this->collide_entity_func = 0;
}

inline int __cdecl CG_DrawSmallDevStringColor(struct ScreenPlacement *scrPlace, float x, float y, const char *s, const float *color, int align)
{
	return CG_DrawDevString(scrPlace, x, y, 1.0, 1.0, s, color, align, font);
}
int __cdecl R_PickMaterial(int traceMask, const float *org, const float *dir, char *name, char *surfaceFlags, char *contents, int charLimit)
{
	float end[3];
	end[0] = dir[0] * 2621440.0f + org[0];
	end[1] = dir[1] * 2621440.0f + org[1];
	end[2] = dir[2] * 2621440.0f + org[2];

	float* vec3_origin = (float*)0x00A5E370;

	col_context_t context;
	trace_t trace;
	trace.normal.vec.v[0] = 0.0f;
	trace.normal.vec.v[1] = 0.0f;
	trace.normal.vec.v[2] = 0.0f;

	CG_TraceCapsule(&trace, org, vec3_origin, vec3_origin, end, 0, 0x680CC31, &context);
	
	if (trace.startsolid || trace.allsolid)
		return 0;
	if (isnan(trace.fraction))
		return 0;

	sprintf_s(name, charLimit, "<???>");

	*surfaceFlags = 0;
	surfaceFlags[charLimit - 1] = 0;

	int flags = (trace.sflags >> 20) & 0x1F;
	if (flags - 1 > 0x1D)
		strncpy_s(surfaceFlags, charLimit, "^1default^7", _TRUNCATE);
	else
		strncpy_s(surfaceFlags, charLimit, _infoParms[flags].name, _TRUNCATE);

	if (surfaceFlags[charLimit - 1])
		return 0;

	int surfaceFlagsLen = strlen(surfaceFlags);
	if (trace.cflags & 1)
		strncpy_s(contents, charLimit, "solid", _TRUNCATE);
	else
		strncpy_s(contents, charLimit, "^3nonsolid^7", _TRUNCATE);

	if (contents[charLimit - 1])
		return 0;

	int contentsLen = strlen(contents);
	for (int i = 30; _infoParms[i].name; i++)
	{
		if (trace.sflags & _infoParms[i].surfaceFlags)
		{
			surfaceFlags[surfaceFlagsLen++] = ' ';
			char* str = &surfaceFlags[surfaceFlagsLen];
			strncpy_s(str, charLimit - surfaceFlagsLen, _infoParms[i].name, _TRUNCATE);

			if (surfaceFlags[charLimit - 1])
				return 0;

			surfaceFlagsLen += strlen(str);
		}

		if (trace.cflags * _infoParms[i].contents)
		{
			contents[contentsLen++] = ' ';
			char* str = &contents[contentsLen];
			strncpy_s(str, charLimit - contentsLen, _infoParms[i].name, _TRUNCATE);

			if (contents[charLimit - 1])
				return 0;

			contentsLen += strlen(str);
		}
	}

	return 1;
}

void CG_DrawMaterial(int localClientNum, int drawMaterialType)
{
	const int charLimit = 4096;

	char name[charLimit] = "\0";
	char surfaceFlags[charLimit] = "\0";
	char contents[charLimit] = "\0";

	float* cgArray = *(float**)0x02FF5354;
	float* viewOrg = &cgArray[143433];
	float* viewAxis = &cgArray[143437];

	struct ScreenPlacement* scrPlaceView = (struct ScreenPlacement*)0x00C78DA0;

	if (R_PickMaterial(NULL, viewOrg, viewAxis, name, surfaceFlags, contents, charLimit))
	{
		struct ScreenPlacement* scrPlace = scrPlaceView;

		float y = CG_DrawSmallDevStringColor(scrPlace, 8.0f, 240.0f, name, colorWhite, 5) + 240.0f;
		y += CG_DrawSmallDevStringColor(scrPlace, 8.0, y, surfaceFlags, colorWhite, 5);
		CG_DrawSmallDevStringColor(scrPlace, 8.0, y, contents, colorWhite, 5);
	}
}



void __declspec(naked) mfh_CG_DrawFullScreenDebugOverlays(void)
{
	static DWORD dwRetn = 0x004075F9;
	_asm
	{
		push [esp] //localClientNum
		call CG_DrawFullScreenDebugOverlays
		add esp, 4

		jmp dwRetn
	}
}

void __cdecl CG_DrawFullScreenDebugOverlays(int localClientNum)
{
	if (cg_drawVersion->current.enabled)
		CG_DrawVersion();
	if (cg_drawMaterial->current.integer > 0)
		CG_DrawMaterial(localClientNum, cg_drawMaterial->current.integer);
}
