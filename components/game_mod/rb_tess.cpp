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

	ASSERT(context.state->material); // This appears to always be null

	MaterialTechniqueSet* techSet;
	MaterialTechnique* tech = Material_GetTechnique(context.state->material, context.state->techType);
	ASSERT(tech);

	const char* infoString;
	const char* infoIdString;

	switch (r_showTess->current.integer)
	{
	case 1:
		infoString = tech->name;
		offsetCenter[2] = ((context.state->techType - 16.0f) * 0.3f) + offsetCenter[2];
		infoIdString = "T";
		break;
	case 4:
		ASSERT(tech->passCount);
		if (tech->passArray[0].vertexShader)
			infoString = tech->passArray[0].vertexShader->name;
		else
			infoString = "<NONE>";
		offsetCenter[2] = ((context.state->techType - 16.0f) * 0.3f) + offsetCenter[2];
		infoIdString = "VS";
		break;
	case 5:
		ASSERT(tech->passCount);
		if (tech->passArray[0].pixelShader)
			infoString = tech->passArray[0].pixelShader->name;
		else
			infoString = "<NONE>";
		offsetCenter[2] = ((context.state->techType - 16.0f) * 0.3f) + offsetCenter[2];
		infoIdString = "PS";
		break;
	case 2:
		techSet = Material_GetTechniqueSet(context.state->material);
		ASSERT(techSet);
		infoString = techSet->name;
		infoIdString = "TS";
		break;
	case 3:
		infoString = context.state->material->info.name;
		infoIdString = "M";
		break;
	default:
		ASSERT_MSG_VA(0, "Unknown value for r_showTess: %i", r_showTess->current.integer);
		infoString = "?";
		infoIdString = "?";
		break;
	}
	GfxBackEndData* data = context.source->input.data;
	const char* str = va("%s:%s=%s", tessName, infoIdString, infoString);
	R_AddDebugString(&data->debugGlobals, offsetCenter, color, TEXT_SIZE, str);
}
