#include "stdafx.h"

bool Material_UsingTechnique_WAW(int techType)
{
	ASSERT(techType < 59);

	return ((bool *)0x0064B67C)[techType];
}

const char *Material_NameForStreamDest_WAW(char dest)
{
	switch (dest)
	{
	case 0: return "position";
	case 1: return "normal";
	case 2: return "color[0]";
	case 3: return "color[1]";
	case 4: return "depth";
	case 5: return "texcoord[0]";
	case 6: return "texcoord[1]";
	case 7: return "texcoord[2]";
	case 8: return "texcoord[3]";
	case 9: return "texcoord[4]";
	case 10:return "texcoord[5]";
	case 11:return "texcoord[6]";
	case 12:return "texcoord[7]";
	case 13:return "blendweight";

	default:
		ASSERT_MSG(false, "Unhandled case");
		break;
	}

	return "";
}

bool Material_StreamDestForName_WAW(const char **text, const char *destName, char *dest)
{
	int index;

	if (!memcmp(destName, "position", 9))
	{
		*dest = 0;
		return true;
	}
	if (!memcmp(destName, "normal", 7))
	{
		*dest = 1;
		return true;
	}
	if (!memcmp(destName, "color", 6))
	{
		if (Material_ParseIndex(text, 2, &index))
		{
			*dest = index + 2;
			return true;
		}
	}
	else
	{
		if (!memcmp(destName, "depth", 6))
		{
			*dest = 4;
			return true;
		}
		if (!memcmp(destName, "texcoord", 9))
		{
			if (Material_ParseIndex(text, 8, &index))
			{
				*dest = index + 5;
				return true;
			}
		}
		else
		{
			if (!memcmp(destName, "blendweight", 12))
			{
				*dest = 13;
				return true;
			}

			Com_ScriptError("Unknown stream destination '%s'\n", destName);
		}
	}

	return false;
}

bool Material_StreamSourceForName_WAW(const char **text, const char *sourceName, char *source)
{
	int index;

	if (!memcmp(sourceName, "position", 9))
	{
		*source = 0;
		return true;
	}
	if (!memcmp(sourceName, "normal", 7))
	{
		*source = 3;
		return true;
	}
	if (!memcmp(sourceName, "tangent", 8))
	{
		*source = 4;
		return true;
	}
	if (!memcmp(sourceName, "color", 6))
	{
		*source = 1;
		return true;
	}
	if (memcmp(sourceName, "texcoord", 9))
	{
		if (!memcmp(sourceName, "normalTransform", 16))
		{
			if (Material_ParseIndex(text, 2, &index))
			{
				*source = index + 7;
				return 1;
			}
		}
		else
		{
			if (!memcmp(sourceName, "blendweight", 12))
			{
				*source = 9;
				return 1;
			}

			Com_ScriptError("Unknown stream source '%s'\n", sourceName);
		}
		return 0;
	}

	if (!Material_ParseIndex(text, 3, &index))
		return false;

	if (index)
	{
		*source = index + 4;
		return true;
	}

	*source = 2;
	return true;
}

char Material_GetStreamDestForSemantic_WAW(D3DXSEMANTIC *semantic)
{
	switch (semantic->Usage)
	{
	case D3DDECLUSAGE_POSITION:
		if (semantic->UsageIndex)
			break;

		return 0;

	case D3DDECLUSAGE_BLENDWEIGHT:
		if (semantic->UsageIndex)
			break;

		return 13;

	case D3DDECLUSAGE_NORMAL:
		if (semantic->UsageIndex)
			break;

		return 1;

	case D3DDECLUSAGE_TEXCOORD:
		if (semantic->UsageIndex >= 8)
			break;

		return semantic->UsageIndex + 5;

	case D3DDECLUSAGE_COLOR:
		if (semantic->UsageIndex >= 2)
			break;

		return semantic->UsageIndex + 2;

	case D3DDECLUSAGE_DEPTH:
		if (semantic->UsageIndex)
			break;

		return 4;
	}

	Com_Error(ERR_DROP, "Unknown shader input/output usage %i:%i\n", semantic->Usage, semantic->UsageIndex);
	return 0;
}

int Material_TechniqueTypeForName_WAW(const char *name)
{
	const char *techniqueNames[59];
	techniqueNames[0] = "\"depth prepass\"";
	techniqueNames[1] = "\"build floatz\"";
	techniqueNames[2] = "\"build shadowmap depth\"";
	techniqueNames[3] = "\"build shadowmap color\"";
	techniqueNames[4] = "\"unlit\"";
	techniqueNames[5] = "\"emissive\"";
	techniqueNames[6] = "\"emissive shadow\"";
	techniqueNames[7] = "\"emissive reflected\"";
	techniqueNames[8] = "\"lit\"";
	techniqueNames[9] = "\"lit fade\"";
	techniqueNames[10] = "\"lit sun\"";
	techniqueNames[11] = "\"lit sun fade\"";
	techniqueNames[12] = "\"lit sun shadow\"";
	techniqueNames[13] = "\"lit sun shadow fade\"";
	techniqueNames[14] = "\"lit spot\"";
	techniqueNames[15] = "\"lit spot fade\"";
	techniqueNames[16] = "\"lit spot shadow\"";
	techniqueNames[17] = "\"lit spot shadow fade\"";
	techniqueNames[18] = "\"lit omni\"";
	techniqueNames[19] = "\"lit omni fade\"";
	techniqueNames[20] = "\"lit omni shadow\"";
	techniqueNames[21] = "\"lit omni shadow fade\"";
	techniqueNames[22] = "\"lit charred\"";
	techniqueNames[23] = "\"lit fade charred\"";
	techniqueNames[24] = "\"lit sun charred\"";
	techniqueNames[25] = "\"lit sun fade charred\"";
	techniqueNames[26] = "\"lit sun shadow charred\"";
	techniqueNames[27] = "\"lit sun shadow fade charred\"";
	techniqueNames[28] = "\"lit spot charred\"";
	techniqueNames[29] = "\"lit spot fade charred\"";
	techniqueNames[30] = "\"lit spot shadow charred\"";
	techniqueNames[31] = "\"lit spot shadow fade charred\"";
	techniqueNames[32] = "\"lit omni charred\"";
	techniqueNames[33] = "\"lit omni fade charred\"";
	techniqueNames[34] = "\"lit omni shadow charred\"";
	techniqueNames[35] = "\"lit omni shadow fade charred\"";
	techniqueNames[36] = "\"lit instanced\"";
	techniqueNames[37] = "\"lit instanced sun\"";
	techniqueNames[38] = "\"lit instanced sun shadow\"";
	techniqueNames[39] = "\"lit instanced spot\"";
	techniqueNames[40] = "\"lit instanced spot shadow\"";
	techniqueNames[41] = "\"lit instanced omni\"";
	techniqueNames[42] = "\"lit instanced omni shadow\"";
	techniqueNames[43] = "\"light spot\"";
	techniqueNames[44] = "\"light omni\"";
	techniqueNames[45] = "\"light spot shadow\"";
	techniqueNames[46] = "\"light spot charred\"";
	techniqueNames[47] = "\"light omni charred\"";
	techniqueNames[48] = "\"light spot shadow charred\"";
	techniqueNames[49] = "\"fakelight normal\"";
	techniqueNames[50] = "\"fakelight view\"";
	techniqueNames[51] = "\"sunlight preview\"";
	techniqueNames[52] = "\"case texture\"";
	techniqueNames[53] = "\"solid wireframe\"";
	techniqueNames[54] = "\"shaded wireframe\"";
	techniqueNames[55] = "\"shadowcookie caster\"";
	techniqueNames[56] = "\"shadowcookie receiver\"";
	techniqueNames[57] = "\"debug bumpmap\"";
	techniqueNames[58] = "\"debug bumpmap instanced\"";

	for (int techniqueIndex = 0; techniqueIndex < 59; techniqueIndex++)
	{
		if (!strcmp(name, techniqueNames[techniqueIndex]))
			return techniqueIndex;
	}

	return 58;
}

/*
#ifndef BO1_BUILD
unsigned int g_customSamplerSrc[3] =
{
	30, 4, 5,
};

unsigned int g_customSamplerDest[3] =
{
	1, 2, 3,
};
#endif
*/