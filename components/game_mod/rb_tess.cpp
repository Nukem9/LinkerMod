#include "stdafx.h"

void __cdecl RB_ShowTess(GfxCmdBufContext context, const float *center, const char *tessName, const float *color)
{
	const float TEXT_SIZE = 0.6f;

	ASSERT(center);
	ASSERT(tessName);

	float offsetCenter[3];
	offsetCenter[0] = center[0];
	offsetCenter[1] = center[1];
	offsetCenter[2] = center[2];

	GfxBackEndData* daata = context.local._s0.source->input.data;
	R_AddDebugString(&daata->debugGlobals, offsetCenter, color, TEXT_SIZE, va("M:0x%X T:0x%X\n", context.local._s0.state->material, context.local._s0.state->technique));
	return;

	ASSERT(context.local._s0.state->material); // This appears to always be null

	MaterialTechniqueSet* techSet;
	MaterialTechnique* tech = Material_GetTechnique(context.local._s0.state->material, context.local._s0.state->techType);
	ASSERT(tech);

	const char* infoString;
	const char* infoIdString;

	switch (r_showTess->current.integer)
	{
	case 1:
		infoString = tech->name;
		offsetCenter[2] = ((context.local._s0.state->techType - 16.0f) * 0.3f) + offsetCenter[2];
		infoIdString = "T";
		break;
	case 4:
		ASSERT(tech->passCount);
		if (tech->passArray[0].vertexShader)
			infoString = tech->passArray[0].vertexShader->name;
		else
			infoString = "<NONE>";
		offsetCenter[2] = ((context.local._s0.state->techType - 16.0f) * 0.3f) + offsetCenter[2];
		infoIdString = "VS";
		break;
	case 5:
		ASSERT(tech->passCount);
		if (tech->passArray[0].pixelShader)
			infoString = tech->passArray[0].pixelShader->name;
		else
			infoString = "<NONE>";
		offsetCenter[2] = ((context.local._s0.state->techType - 16.0f) * 0.3f) + offsetCenter[2];
		infoIdString = "PS";
		break;
	case 2:
		techSet = Material_GetTechniqueSet(context.local._s0.state->material);
		ASSERT(techSet);
		infoString = techSet->name;
		infoIdString = "TS";
		break;
	case 3:
		infoString = context.local._s0.state->material->info.name;
		infoIdString = "M";
		break;
	default:
		ASSERT_MSG(0, va("Unknown value for r_showTess: %i", r_showTess));
		infoString = "?";
		infoIdString = "?";
		break;
	}
	GfxBackEndData* data = context.local._s0.source->input.data;
	const char* str = va("%s:%s=%s", tessName, infoIdString, infoString);
	R_AddDebugString(&data->debugGlobals, offsetCenter, color, TEXT_SIZE, str);
}
