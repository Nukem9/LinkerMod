#include "stdafx.h"

#define STREAM_SRC_COUNT 10

SRCLINE(900)
LPDIRECT3DVERTEXDECLARATION9 Material_BuildVertexDecl(MaterialStreamRouting *routingData, int streamCount, stream_source_info_t *sourceTable)
{
	CHECK_SIZE(D3DVERTEXELEMENT9, 8);

	//
	// Vertex declaration array
	//
	D3DVERTEXELEMENT9 elemTable[256];
	memset(elemTable, 0, sizeof(elemTable));

	//
	// Initialize the ending marker with default values
	//
	D3DVERTEXELEMENT9 endDecl = D3DDECL_END();

	int elemIndex = 0;

	printf("new -- 0x%X\n", elemTable);
	while (streamCount > 0)
	{
		ASSERT_MSG((unsigned)(routingData->source) < (unsigned)(STREAM_SRC_COUNT), "routingData->source doesn't index STREAM_SRC_COUNT\n\t%i not in [0, 10)");

		stream_source_info_t *sourceInfo = &sourceTable[routingData->source];

		if (sourceInfo->Stream == SAMPLER_INDEX_INVALID)
			return nullptr;

		stream_dest_info_t *destInfo = &s_streamDestInfo[routingData->dest];

		int elemIndexInsert = elemIndex;
		for (; elemIndexInsert > 0 && elemTable[elemIndexInsert].Stream > sourceInfo->Stream; elemIndexInsert--)
			memcpy(&elemTable[elemIndexInsert], &endDecl, sizeof(D3DVERTEXELEMENT9));

		printf("Type: %d %d\n", sourceInfo->Type, destInfo->Usage);

		elemTable[elemIndexInsert].Stream		= sourceInfo->Stream;
		elemTable[elemIndexInsert].Offset		= sourceInfo->Offset;
		elemTable[elemIndexInsert].Type			= sourceInfo->Type;
		elemTable[elemIndexInsert].Method		= D3DDECLMETHOD_DEFAULT;
		elemTable[elemIndexInsert].Usage		= destInfo->Usage;
		elemTable[elemIndexInsert].UsageIndex	= destInfo->UsageIndex;

		elemIndex++;
		routingData++;
		streamCount--;
	}

	//
	// Copy the ending marker over, to the actual array end
	//
	memcpy(&elemTable[elemIndex], &endDecl, sizeof(D3DVERTEXELEMENT9));

	IDirect3DDevice9 *d3d_device = *(IDirect3DDevice9 **)0x13A15A0;

	if (d3d_device)
	{
		LPDIRECT3DVERTEXDECLARATION9 decl	= nullptr;
		HRESULT hr							= d3d_device->CreateVertexDeclaration(elemTable, &decl);

		if (!SUCCEEDED(hr))
		{
			// g_disableRendering++;
			(*(DWORD *)0xEE4F80)++;

			Com_Error(ERR_FATAL, "dx.device->CreateVertexDeclaration(elemTable, &decl) failed: %s\n", R_ErrorDescription(hr));
		}

		ASSERT(decl != nullptr);
		return decl;
	}

	return nullptr;
}

SRCLINE(962)
void Load_BuildVertexDecl(MaterialVertexDeclaration **mtlVertDecl)
{
	MaterialStreamRouting data[16];
	memcpy(data, &(*mtlVertDecl)->routing, sizeof(data));

	for (int vertDeclType = 0; vertDeclType < 18; vertDeclType++)
	{
		// r_loadForRenderer->current.enabled
		if (*(BYTE *)(*(DWORD *)0x0191D574 + 12))
		{
			(*mtlVertDecl)->routing.decl[vertDeclType] = Material_BuildVertexDecl(
				data,
				(*mtlVertDecl)->streamCount,
				s_streamSourceInfo[vertDeclType]);
		}
		else
		{
			(*mtlVertDecl)->routing.decl[vertDeclType] = nullptr;
		}
	}

	(*mtlVertDecl)->isLoaded = true;
}

SRCLINE(2094)
unsigned int Material_HashVertexDecl(MaterialStreamRouting *routingData, int streamCount)
{
	unsigned int hash = 0;

	for (int byteIndex = 0; byteIndex < 2 * streamCount; byteIndex++)
		hash += (byteIndex + 0x77) * routingData->data[byteIndex];

	return hash & 63;
}

SRCLINE(2115)
MaterialVertexDeclaration *Material_AllocVertexDecl(MaterialStreamRouting *routingData, int streamCount, bool *existing)
{
	ASSERT(streamCount);

	//
	// Static array of vertex declarations (NOTE: WAW struct size is smaller)
	//
	static MaterialVertexDeclaration materialVertexDeclarations[64];
	static int materialVertexDeclCount = 0;

	//
	// Calculate a hash for lookup
	//
	unsigned int hashIndex = Material_HashVertexDecl(routingData, streamCount);

	//
	// Iterate all possible declaration slots
	//
	MaterialVertexDeclaration *mvd = &materialVertexDeclarations[hashIndex];

	for (; mvd->streamCount; mvd = &materialVertexDeclarations[hashIndex])
	{
		if (mvd->streamCount == streamCount && !memcmp(&mvd->routing, routingData, 2 * streamCount))
		{
			*existing = true;
			return mvd;
		}

		hashIndex = (hashIndex + 1) & 63;
	}

	//
	// Maximum index check
	//
	if (materialVertexDeclCount == 63)
		Com_Error(ERR_DROP, "More than %i vertex declarations in use", 63);

	materialVertexDeclCount++;

	//
	// Zero the struct and then copy the routing data
	//
	memset(&mvd->streamCount, 0, sizeof(MaterialVertexDeclaration));
	memcpy(&mvd->routing, &routingData->source, 2 * streamCount);

	//
	// Sanity checks
	//
	ASSERT_MSG(streamCount < ARRAYSIZE(mvd->routing.data), "streamCount doesn't index ARRAY_COUNT(mvd->routing.data)");

	mvd->streamCount = streamCount;

	ASSERT(mvd->streamCount == streamCount);

	//
	// Set flags for any optional vertex routings
	//
	for (int routingIndex = 0; routingIndex < streamCount; routingIndex++)
	{
		if (mvd->routing.data[routingIndex].source >= 5)
		{
			mvd->hasOptionalSource = true;
			break;
		}
	}

	*existing = false;
	return mvd;
}

SRCLINE(2158)
const float *Material_RegisterLiteral(const float *literal)
{
	static DWORD dwCall = 0x0052CC60;

	__asm
	{
		mov edi, literal
		call [dwCall]
	}
}

SRCLINE(2197)
const char *Material_RegisterString(const char *string)
{
	return ((const char *(__cdecl *)(const char *))0x0052CD70)(string);
}

SRCLINE(2819)
bool Material_UsingTechnique(int techType)
{
#ifndef BO1_BUILD
	return Material_UsingTechnique_WAW(techType);
#else
	ASSERT(techType < ARRAYSIZE(g_useTechnique));

	return g_useTechnique[techType];
#endif
}

SRCLINE(2825)
bool Material_MatchToken(const char **text, const char *match)
{
	return Com_MatchToken(text, match, 1) != 0;
}

SRCLINE(3124)
MaterialStateMap *Material_RegisterStateMap(const char *name)
{
	static DWORD dwCall = 0x0052D950;

	__asm
	{
		mov eax, name
		call [dwCall]
	}
}

SRCLINE(3140)
bool Material_LoadPassStateMap(const char **text, MaterialStateMap **stateMap)
{
	if (!Material_MatchToken(text, "stateMap"))
		return false;

	const char *token = Com_Parse(text);

	if (!*token || *token == ';')
	{
		Com_ScriptError("missing stateMap filename\n");
		return false;
	}

	*stateMap = Material_RegisterStateMap(token);

	if (*stateMap)
		return Material_MatchToken(text, ";");

	return false;
}

SRCLINE(3158)
bool Material_ParseIndex(const char **text, int indexCount, int *index)
{
	if (!Material_MatchToken(text, "["))
		return false;

	*index = Com_ParseInt(text);

	if (*index < 0 || *index >= indexCount)
		Com_ScriptError("index '%i' is not in the range [0, %i]\n", *index, indexCount - 1);

	return Material_MatchToken(text, "]");
}

SRCLINE(3168)
const char *Material_NameForStreamDest(char dest)
{
#ifndef BO1_BUILD
	return Material_NameForStreamDest_WAW(dest);
#else
	switch (dest)
	{
	case 0:	return "position";
	case 1:	return "normal";
	case 2:	return "color[0]";
	case 3:	return "color[1]";
	case 4:	return "depth";
	case 5:	return "texcoord[0]";
	case 6:	return "texcoord[1]";
	case 7:	return "texcoord[2]";
	case 8:	return "texcoord[3]";
	case 9:	return "texcoord[4]";
	case 10:return "texcoord[5]";
	case 11:return "texcoord[6]";
	case 12:return "texcoord[7]";
	case 13:return "texcoord[8]";
	case 14:return "texcoord[9]";
	case 15:return "texcoord[10]";
	case 16:return "texcoord[11]";
	case 17:return "texcoord[12]";
	case 18:return "texcoord[13]";
	case 19:return "blendweight";

	default:
		ASSERT_MSG(false, "Unhandled case");
		break;
	}

	return "";
#endif
}

SRCLINE(3221)
bool Material_StreamDestForName(const char **text, const char *destName, char *dest)
{
#ifndef BO1_SEMI_NATIVE_BUILD
	return Material_StreamDestForName_WAW(text, destName, dest);
#else
	int index;

	if (!strcmp(destName, "position"))
		*dest = 0;
	else if (!strcmp(destName, "normal"))
		*dest = 1;
	else if (!strcmp(destName, "color"))
	{
		if (!Material_ParseIndex(text, 2, &index))
			return false;

		*dest = index + 2;
	}
	else if (!strcmp(destName, "depth"))
		*dest = 4;
	else if (!strcmp(destName, "texcoord"))
	{
		if (!Material_ParseIndex(text, 14, &index))
			return false;

		*dest = index + 5;
	}
	else if (!strcmp(destName, "blendweight"))
		*dest = 19;
	else
	{
		Com_ScriptError("Unknown stream destination '%s'\n", destName);
		return false;
	}

	return true;
#endif
}

SRCLINE(3263)
bool Material_StreamSourceForName(const char **text, const char *sourceName, char *source)
{
#ifndef BO1_SEMI_NATIVE_BUILD
	return Material_StreamSourceForName_WAW(text, sourceName, source);
#else
	int index = 0;

	if (!strcmp(sourceName, "position"))
		*source = 0;
	else if (!strcmp(sourceName, "normal"))
		*source = 3;
	else if (!strcmp(sourceName, "tangent"))
		*source = 4;
	else if (!strcmp(sourceName, "color"))
		*source = 1;
	else if (!strcmp(sourceName, "texcoord"))
	{
		if (!Material_ParseIndex(text, 3, &index))
			return false;

		if (index)
			*source = index + 4;
		else
			*source = 2;
	}
	else if (!strcmp(sourceName, "normalTransform"))
	{
		if (!Material_ParseIndex(text, 2, &index))
			return false;

		*source = index + 7;
	}
	else if (!strcmp(sourceName, "blendweight"))
		*source = 9;
	else
	{
		Com_ScriptError("unknown stream source '%s'\n", sourceName);
		return false;
	}

	return true;
#endif
}

SRCLINE(3314)
bool Material_ResourceDestForStreamDest(char streamDest, ShaderVaryingDef *inputTable, unsigned int inputCount, char *resourceDest)
{
	unsigned int inputIndex = 0;

	for (;; inputIndex++)
	{
		if (inputIndex >= inputCount)
		{
			Com_ScriptError("vertex shader doesn't use input '%s'.\n", Material_NameForStreamDest(streamDest));
			return false;
		}

		if (inputTable[inputIndex].streamDest == streamDest)
			break;
	}

	if (inputTable[inputIndex].isAssigned)
	{
		Com_ScriptError("vertex input '%s' specified more than once.\n", inputTable[inputIndex].name);
		return false;
	}

	inputTable[inputIndex].isAssigned	= true;
	*resourceDest						= inputTable[inputIndex].resourceDest;
	return true;
}

SRCLINE(3337)
bool Material_CheckUnspecifiedVertexInputs(ShaderVaryingDef *inputTable, unsigned int inputCount)
{
	bool isValid = true;

	for (unsigned int inputIndex = 0; inputIndex < inputCount; inputIndex++)
	{
		if (!inputTable[inputIndex].isAssigned)
		{
			Com_ScriptError("vertex input '%s' is not specified.\n", inputTable[inputIndex].name);
			isValid = false;
		}
	}

	return isValid;
}

SRCLINE(3358)
bool Material_LoadPassVertexDecl(const char **text, ShaderVaryingDef *inputTable, unsigned int inputCount, MaterialPass *pass)
{
	//
	// Material pass vertex stream routing
	//
	MaterialStreamRouting routing[16];
	memset(routing, 0, sizeof(routing));

	int routingIndex = 0;
	for (;; routingIndex++)
	{
		if (routingIndex >= ARRAYSIZE(routing))
		{
			Com_ScriptError("More than %i vertex mappings\n", routingIndex);
			return false;
		}

		if (strcmp(Com_Parse(text), "vertex"))
			break;

		if (!Material_MatchToken(text, "."))
			return false;

		if (!Material_StreamDestForName(text, Com_Parse(text), &routing[0].dest))
			return false;

		char resourceDest;
		if (!Material_ResourceDestForStreamDest(routing[0].dest, inputTable, inputCount, &resourceDest))
			return false;

		if (!Material_MatchToken(text, "=") || !Material_MatchToken(text, "code") || !Material_MatchToken(text, "."))
			return false;

		char source;
		if (!Material_StreamSourceForName(text, Com_Parse(text), &source))
			return false;

		if (!Material_MatchToken(text, ";"))
			return false;

		int insertIndex = routingIndex;
		for (;
			insertIndex > 0
			&& routing[insertIndex].source	>= source
			&& (routing[insertIndex].source != source
			|| routing[insertIndex].dest	>= resourceDest);
			insertIndex--)
			routing[insertIndex + 1] = routing[insertIndex];

		routing[insertIndex + 1].source = source;
		routing[insertIndex + 1].dest	= resourceDest;
	}

	Com_UngetToken();

	if (!Material_CheckUnspecifiedVertexInputs(inputTable, inputCount))
		return false;

	bool existing		= false;
	pass->vertexDecl	= Material_AllocVertexDecl(&routing[1], routingIndex, &existing);

	if (!existing)
		Load_BuildVertexDecl(&pass->vertexDecl);

	return true;
}

SRCLINE(3419)
bool Material_DefaultIndexRange(ShaderIndexRange *indexRangeRef, unsigned int arrayCount, ShaderIndexRange *indexRangeSet)
{
	if (arrayCount)
	{
		if (indexRangeRef->count + indexRangeRef->first > arrayCount)
			return false;
	}
	else if (indexRangeRef->first || indexRangeRef->count != 1)
		return false;

	indexRangeSet->first		= indexRangeRef->first;
	indexRangeSet->count		= indexRangeRef->count;
	indexRangeSet->isImplicit	= false;
	return true;
}

SRCLINE(3437)
bool Material_ParseIndexRange(const char **text, unsigned int arrayCount, ShaderIndexRange *indexRange)
{
	if (*Com_Parse(text) != '[')
	{
		Com_UngetToken();

		indexRange->first		= 0;
		indexRange->count		= arrayCount;
		indexRange->isImplicit	= true;
		return true;
	}

	indexRange->isImplicit	= false;
	indexRange->first		= Com_ParseInt(text);

	if (indexRange->first < arrayCount)
	{
		if (*Com_Parse(text) == ',')
		{
			unsigned int last = Com_ParseInt(text);

			if (last >= indexRange->first && last < arrayCount)
				return Material_MatchToken(text, "]");

			Com_ScriptError("ending index %i is not in the range [%i, %i]\n", last, indexRange->first, arrayCount - 1);
			return false;
		}
		else
		{
			Com_UngetToken();

			indexRange->count = 1;
			return true;
		}
	}

	Com_ScriptError("index %i is not in the range [0, %i]\n", indexRange->first, arrayCount - 1);
	return false;
}

SRCLINE(3480)
void *BufferOffset(void *buffer, int offset)
{
	return (char *)buffer + offset;
}

SRCLINE(3487)
bool Material_ParseArrayOffset(const char **text, int arrayCount, int arrayStride, int *offset)
{
	if (!Material_MatchToken(text, "["))
		return false;

	int arrayIndex = Com_ParseInt(text);

	if (arrayIndex >= 0 && arrayIndex < arrayCount)
	{
		if (!Material_MatchToken(text, "]"))
			return false;

		*offset = arrayStride * arrayIndex;
		return true;
	}

	Com_ScriptError("array index must be in range [0, %i]\n", arrayCount - 1);
	return false;
}

SRCLINE(3506)
bool Material_CodeSamplerSource_r(const char **text, int offset, CodeSamplerSource *sourceTable, ShaderArgumentSource *argSource)
{
	ASSERT(text != nullptr);
	ASSERT(sourceTable != nullptr);

	if (!Material_MatchToken(text, "."))
		return false;

	const char *token = Com_Parse(text);

	int sourceIndex = 0;
	for (;; sourceIndex++)
	{
		if (!sourceTable[sourceIndex].name)
		{
			Com_ScriptError("unknown sampler source '%s'\n", token);
			return false;
		}

		if (!strcmp(token, sourceTable[sourceIndex].name))
			break;
	}

	if (sourceTable[sourceIndex].subtable)
	{
		if (sourceTable[sourceIndex].arrayCount)
		{
			int additionalOffset;

			if (!Material_ParseArrayOffset(
				text,
				sourceTable[sourceIndex].arrayCount,
				sourceTable[sourceIndex].arrayStride,
				&additionalOffset))
				return false;

			offset += additionalOffset;
		}

		return Material_CodeSamplerSource_r(text, offset, sourceTable[sourceIndex].subtable, argSource);
	}
	else
	{
		argSource->type			= MTL_ARG_CODE_PIXEL_SAMPLER;
		argSource->u.codeIndex	= offset + sourceTable[sourceIndex].source;

		if (sourceTable[sourceIndex].arrayCount)
		{
			ASSERT(sourceTable[sourceIndex].arrayStride == 1);
			return Material_ParseIndexRange(text, sourceTable[sourceIndex].arrayCount, &argSource->indexRange);
		}

		argSource->indexRange.first			= 0;
		argSource->indexRange.count			= 1;
		argSource->indexRange.isImplicit	= true;
		return true;
	}

	return false;
}

SRCLINE(3555)
bool Material_ParseSamplerSource(const char **text, ShaderArgumentSource *argSource)
{
	const char *token = Com_Parse(text);

#ifdef BO1_BUILD
	if (!strcmp(token, "sampler"))
		return Material_CodeSamplerSource_r(text, 0, s_codeSamplers, argSource);
#else
	if (!strcmp(token, "sampler"))
		return Material_CodeSamplerSource_r(text, 0, (CodeSamplerSource *)0x0064B7B0, argSource);
#endif

	if (!strcmp(token, "material"))
	{
		if (!Material_MatchToken(text, "."))
			return false;
		
		argSource->type						= MTL_ARG_MATERIAL_PIXEL_SAMPLER;
		argSource->u.name					= Material_RegisterString(Com_Parse(text));
		argSource->indexRange.first			= 0;
		argSource->indexRange.count			= 1;
		argSource->indexRange.isImplicit	= true;

		return argSource->u.name != nullptr;
	}

	Com_ScriptError("expected 'sampler' or 'material', found '%s' instead\n", token);
	return false;
}

SRCLINE(3580)
bool Material_DefaultSamplerSourceFromTable(const char *constantName, ShaderIndexRange *indexRange, CodeSamplerSource *sourceTable, ShaderArgumentSource *argSource)
{
	ASSERT(constantName != nullptr);
	ASSERT(sourceTable != nullptr);
	ASSERT(indexRange != nullptr);
	ASSERT(argSource != nullptr);

	for (int sourceIndex = 0; sourceTable[sourceIndex].name; sourceIndex++)
	{
		if (!sourceTable[sourceIndex].subtable
			&& !strcmp(constantName, sourceTable[sourceIndex].name)
			&& Material_DefaultIndexRange(indexRange, sourceTable[sourceIndex].arrayCount, &argSource->indexRange))
		{
			argSource->type			= MTL_ARG_CODE_PIXEL_SAMPLER;
			argSource->u.codeIndex	= LOWORD(sourceTable[sourceIndex].source);
			return true;
		}
	}

	return false;
}

SRCLINE(3606)
bool Material_DefaultSamplerSource(const char *constantName, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource)
{
#ifdef BO1_BUILD
	return Material_DefaultSamplerSourceFromTable(constantName, indexRange, s_defaultCodeSamplers, argSource);
#else
	if (!Material_DefaultSamplerSourceFromTable(constantName, indexRange, (CodeSamplerSource *)0x0064B9A8, argSource))
		return Material_DefaultSamplerSourceFromTable("modelLightingSampler", indexRange, (CodeSamplerSource *)0x0064B9A8, argSource);
#endif

	return true;
}

SRCLINE(3613)
bool Material_ParseVector(const char **text, int elemCount, float *vector)
{
	if (Material_MatchToken(text, "("))
	{
		int elemIndex = 0;

		while (true)
		{
			vector[elemIndex++] = Com_ParseFloat(text);

			if (elemIndex == elemCount)
				break;

			if (!Material_MatchToken(text, ","))
				return false;
		}

		return Material_MatchToken(text, ")");
	}

	return false;
}

SRCLINE(3632)
bool Material_ParseLiteral(const char **text, const char *token, float *literal)
{
	ASSERT(text != nullptr);
	ASSERT(token != nullptr);
	ASSERT(literal != nullptr);

	literal[0] = 0.0f;
	literal[1] = 0.0f;
	literal[2] = 0.0f;
	literal[3] = 1.0f;

	if (!strcmp(token, "float1"))
		Material_ParseVector(text, 1, literal);
	else if (!strcmp(token, "float2"))
		Material_ParseVector(text, 2, literal);
	else if (!strcmp(token, "float3"))
		Material_ParseVector(text, 3, literal);
	else if (!strcmp(token, "float4"))
		Material_ParseVector(text, 4, literal);
	else
		return false;

	return true;
}

SRCLINE(3657)
bool Material_ParseCodeConstantSource_r(MaterialShaderType shaderType, const char **text, int offset, CodeConstantSource *sourceTable, ShaderArgumentSource *argSource)
{
	ASSERT(text != nullptr);
	ASSERT(sourceTable != nullptr);
	ASSERT(argSource != nullptr);

	if (!Material_MatchToken(text, "."))
		return false;

	const char *token = Com_Parse(text);

	int sourceIndex = 0;
	for (;; sourceIndex++)
	{
		if (!sourceTable[sourceIndex].name)
		{
			Com_ScriptError("unknown constant source '%s'\n", token);
			return false;
		}

		if (!strcmp(token, sourceTable[sourceIndex].name))
			break;
	}

	if (sourceTable[sourceIndex].arrayCount)
	{
		ASSERT(sourceTable[sourceIndex].subtable || (sourceTable[sourceIndex].source < 0xC2 && sourceTable[sourceIndex].arrayStride == 1));
		//ASSERT(sourceTable[sourceIndex].subtable || (sourceTable[sourceIndex].source < CONST_SRC_FIRST_CODE_MATRIX && sourceTable[sourceIndex].arrayStride == 1));

		if (sourceTable[sourceIndex].subtable)
		{
			int additionalOffset;

			if (!Material_ParseArrayOffset(
				text,
				sourceTable[sourceIndex].arrayCount,
				sourceTable[sourceIndex].arrayStride,
				&additionalOffset))
				return false;

			offset += additionalOffset;
		}
		else
		{
			ASSERT(sourceTable[sourceIndex].arrayStride == 1);

			if (!Material_ParseIndexRange(text, sourceTable[sourceIndex].arrayCount, &argSource->indexRange))
				return false;
		}
	}

	if (sourceTable[sourceIndex].subtable)
		return Material_ParseCodeConstantSource_r(shaderType, text, offset, sourceTable[sourceIndex].subtable, argSource);

	argSource->type			= (shaderType == MTL_VERTEX_SHADER) ? MTL_ARG_CODE_VERTEX_CONST : MTL_ARG_CODE_PIXEL_CONST;
	argSource->u.codeIndex	= offset + sourceTable[sourceIndex].source;

	ASSERT((argSource->type == MTL_ARG_CODE_VERTEX_CONST) || s_codeConstUpdateFreq[argSource->u.codeIndex] != MTL_UPDATE_PER_PRIM);

	if (!sourceTable[sourceIndex].arrayCount)
	{
		if (argSource->u.codeIndex >= R_MAX_CODE_INDEX)
		{
			if (!Material_ParseIndexRange(text, 4, &argSource->indexRange))
				return false;
		}
		else
		{
			argSource->indexRange.first			= 0;
			argSource->indexRange.count			= 1;
			argSource->indexRange.isImplicit	= false;
		}
	}

	return true;
}

SRCLINE(3722)
bool Material_ParseConstantSource(MaterialShaderType shaderType, const char **text, ShaderArgumentSource *argSource)
{
	const char *token = Com_Parse(text);

	float literal[4];
	if (Material_ParseLiteral(text, token, literal))
	{
		argSource->type						= (shaderType == MTL_VERTEX_SHADER) ? MTL_ARG_LITERAL_VERTEX_CONST : MTL_ARG_LITERAL_PIXEL_CONST;
		argSource->u.literalConst			= Material_RegisterLiteral(literal);
		argSource->indexRange.first			= 0;
		argSource->indexRange.count			= 1;
		argSource->indexRange.isImplicit	= true;

		return argSource->u.literalConst != nullptr;
	}

#ifdef BO1_BUILD
	if (!strcmp(token, "constant"))
		return Material_ParseCodeConstantSource_r(shaderType, text, 0, s_codeConsts, argSource);
#else
	if (!strcmp(token, "constant"))
		return Material_ParseCodeConstantSource_r(shaderType, text, 0, (CodeConstantSource *)0x0064BCD0, argSource);
#endif

	if (!strcmp(token, "material"))
	{
		if (!Material_MatchToken(text, "."))
			return false;

		token = Com_Parse(text);
		argSource->type						= (shaderType == MTL_VERTEX_SHADER) ? MTL_ARG_MATERIAL_VERTEX_CONST : MTL_ARG_MATERIAL_PIXEL_CONST;
		argSource->u.name					= Material_RegisterString(token);
		argSource->indexRange.first			= 0;
		argSource->indexRange.count			= 1;
		argSource->indexRange.isImplicit	= true;

		return argSource->u.name != nullptr;
	}

	Com_ScriptError("expected 'sampler' or 'material', found '%s' instead\n", token);
	return false;
}

SRCLINE(3758)
bool Material_DefaultConstantSourceFromTable(MaterialShaderType shaderType, const char *constantName, ShaderIndexRange *indexRange, CodeConstantSource *sourceTable, ShaderArgumentSource *argSource)
{
	printf("CONST: %s\n", constantName);

	int sourceIndex = 0;
	for (;; sourceIndex++)
	{
		if (!sourceTable[sourceIndex].name)
			return false;

		if (!sourceTable[sourceIndex].subtable && !strcmp(constantName, sourceTable[sourceIndex].name))
		{
			unsigned int arrayCount;

			printf("MATCH: %s\n", constantName);

			if (sourceTable[sourceIndex].source < R_MAX_CODE_INDEX)
			{
				int count	= (sourceTable[sourceIndex].arrayCount > 1) ? sourceTable[sourceIndex].arrayCount : 1;
				arrayCount	= count;
			}
			else
			{
				ASSERT(sourceTable[sourceIndex].arrayCount == 0);

				arrayCount = 4;
			}

			if (Material_DefaultIndexRange(indexRange, arrayCount, &argSource->indexRange))
				break;
		}
	}

	argSource->type			= (shaderType == MTL_VERTEX_SHADER) ? MTL_ARG_CODE_VERTEX_CONST : MTL_ARG_CODE_PIXEL_CONST;
	argSource->u.codeIndex	= sourceTable[sourceIndex].source;

	ASSERT((argSource->type == MTL_ARG_CODE_VERTEX_CONST) || s_codeConstUpdateFreq[argSource->u.codeIndex] != MTL_UPDATE_PER_PRIM);
	return true;
}

const char *remap[] =
{
	"fogConsts2",
	"sunFog",
	"sunFogColor",
	"sunFogDir",
	"hdrControl0",
	"lightAttenuation",
	"lightConeControl1",
	"lightConeControl2",
	"lightFallOffA",
	"lightFallOffB",
	"lightSpotAABB",
	"lightSpotCookieSlideControl",
	"lightSpotMatrix0",
	"lightSpotMatrix1",
	"lightSpotMatrix2",
	"lightSpotMatrix3",
};

SRCLINE(3791)
bool Material_DefaultConstantSource(MaterialShaderType shaderType, const char *constantName, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource)
{

	for (int i = 0; i < ARRAYSIZE(remap); i++)
	{
		if (strstr(constantName, remap[i]))
		{
			constantName = "fogConsts";
			break;
		}
	}

#ifdef BO1_BUILD
	if (Material_DefaultConstantSourceFromTable(shaderType, constantName, indexRange, s_codeConsts, argSource))
		return true;

	return Material_DefaultConstantSourceFromTable(shaderType, constantName, indexRange, s_defaultCodeConsts, argSource);
#else
	if (Material_DefaultConstantSourceFromTable(shaderType, constantName, indexRange, (CodeConstantSource *)0x0064BCD0, argSource))
		return true;

	if (!Material_DefaultConstantSourceFromTable(shaderType, constantName, indexRange, (CodeConstantSource *)0x0064C558, argSource))
		return Material_DefaultConstantSource(shaderType, "fogConsts", indexRange, argSource);
#endif

	return true;
}

SRCLINE(3800)
bool Material_UnknownShaderworksConstantSource(MaterialShaderType shaderType, const char *constantName, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource)
{
	if (I_strnicmp(constantName, "__", 2))
		return false;

	float literalVal[4];
	literalVal[0] = 0.0f;
	literalVal[1] = 0.0f;
	literalVal[2] = 0.0f;
	literalVal[3] = 0.0f;

	argSource->type						= (shaderType == MTL_VERTEX_SHADER) ? MTL_ARG_LITERAL_VERTEX_CONST : MTL_ARG_LITERAL_PIXEL_CONST;
	argSource->u.literalConst			= Material_RegisterLiteral(literalVal);
	argSource->indexRange.first			= indexRange->first;
	argSource->indexRange.count			= indexRange->count;
	argSource->indexRange.isImplicit	= true;

	return argSource->u.literalConst != nullptr;
}

SRCLINE(3815)
unsigned int Material_ElemCountForParamName(const char *shaderName, ShaderUniformDef *paramTable, unsigned int paramCount, const char *name, ShaderParamType *paramType)
{
	unsigned int count = 0;

	for (unsigned int paramIndex = 0; paramIndex < paramCount; paramIndex++)
	{
		if (!strcmp(name, paramTable[paramIndex].name))
		{
			if (count && paramTable[paramIndex].type != *paramType)
				Com_Error(ERR_DROP, "param type changed from %i to %i", paramTable[paramIndex].type, *paramType);

			*paramType = paramTable[paramIndex].type;

			if (count <= paramTable[paramIndex].index)
				count = paramTable[paramIndex].index + 1;
			
			ASSERT(count > 0);
		}
	}

	return count;
}

SRCLINE(3840)
bool Material_ParseArgumentSource(MaterialShaderType shaderType, const char **text, const char *shaderName, ShaderParamType paramType, ShaderArgumentSource *argSource)
{
	ASSERT(text != nullptr);
	ASSERT(*text != '\0');
	ASSERT(shaderName != nullptr);
	ASSERT(argSource != nullptr);

	if (Material_MatchToken(text, "="))
	{
		if (paramType)
		{
			if (paramType > SHADER_PARAM_FLOAT4 && paramType <= SHADER_PARAM_SAMPLER_1D)
				return Material_ParseSamplerSource(text, argSource);
		
			ASSERT_MSG(false, "Unknown constant type");
			return false;
		}

		return Material_ParseConstantSource(shaderType, text, argSource);
	}

	return false;
}

SRCLINE(3868)
bool Material_DefaultArgumentSource(MaterialShaderType shaderType, const char *constantName, ShaderParamType paramType, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource)
{
	ASSERT(constantName != nullptr);
	ASSERT(argSource != nullptr);

	printf("type: %d name: %s param: %d\n", shaderType, constantName, paramType);

	if (paramType)
	{
		if (paramType > SHADER_PARAM_FLOAT4 && paramType <= SHADER_PARAM_SAMPLER_1D)
			return Material_DefaultSamplerSource(constantName, indexRange, argSource);

		ASSERT_MSG(false, "Unknown default constant type");
		return false;
	}

	if (Material_DefaultConstantSource(shaderType, constantName, indexRange, argSource))
		return true;

	return Material_UnknownShaderworksConstantSource(shaderType, constantName, indexRange, argSource);
}

SRCLINE(3895)
unsigned int R_SetParameterDefArray(LPD3DXSHADER_CONSTANTTABLE constantTable, unsigned int constantIndex, ShaderUniformDef *paramDef)
{
	CHECK_SIZE(D3DXSHADER_CONSTANTINFO, 20);

	LPD3DXSHADER_CONSTANTINFO constantInfoTable = (LPD3DXSHADER_CONSTANTINFO)BufferOffset(constantTable, constantTable->ConstantInfo);
	LPD3DXSHADER_CONSTANTINFO constantInfo		= &constantInfoTable[constantIndex];

	LPD3DXSHADER_TYPEINFO typeInfo	= (LPD3DXSHADER_TYPEINFO)BufferOffset(constantTable, constantInfo->TypeInfo);
	const char *name				= (const char *)BufferOffset(constantTable, constantInfo->Name);
	bool isTransposed				= typeInfo->Class == D3DXPC_MATRIX_COLUMNS;

	ShaderParamType type;
	switch (typeInfo->Type)
	{
	case D3DXPT_BOOL:		type = SHADER_PARAM_FLOAT4;			break;
	case D3DXPT_FLOAT:		type = SHADER_PARAM_FLOAT4;			break;
	case D3DXPT_SAMPLER1D:	type = SHADER_PARAM_SAMPLER_1D;		break;
	case D3DXPT_SAMPLER2D:	type = SHADER_PARAM_SAMPLER_2D;		break;
	case D3DXPT_SAMPLER3D:	type = SHADER_PARAM_SAMPLER_3D;		break;
	case D3DXPT_SAMPLERCUBE:type = SHADER_PARAM_SAMPLER_CUBE;	break;

	default:
		Com_ScriptError("Unknown constant type '%i' (%s)\n", typeInfo->Type, name);
		return 0;
	}

	unsigned int paramDefIndex;
	for (paramDefIndex = 0; paramDefIndex < constantInfo->RegisterCount; paramDefIndex++)
	{
		paramDef->type			= type;
		paramDef->name			= name;
		paramDef->index			= paramDefIndex;
		paramDef->resourceDest	= paramDefIndex + constantInfo->RegisterIndex;
		paramDef->isTransposed	= isTransposed;
		paramDef->isAssigned	= false;
		paramDef++;
	}

	return paramDefIndex;
}

SRCLINE(3967)
int Material_PrepareToParseShaderArguments(D3DXSHADER_CONSTANTTABLE *constantTable, ShaderUniformDef *paramTable)
{
	int usedCount = 0;

	printf("const count: %d\n", constantTable->Constants);

	for (unsigned int constantIndex = 0; constantIndex < constantTable->Constants; constantIndex++)
		usedCount += R_SetParameterDefArray(constantTable, constantIndex, &paramTable[usedCount]);

	printf("used count: %d\n", usedCount);

	return usedCount;
}

SRCLINE(3980)
int Material_CompareShaderArgumentsForCombining(const void *e0, const void *e1)
{
	MaterialShaderArgument *c1 = (MaterialShaderArgument *)e0;
	MaterialShaderArgument *c2 = (MaterialShaderArgument *)e1;

	int v4 = c1->type == MTL_ARG_CODE_PIXEL_SAMPLER || c1->type == MTL_ARG_MATERIAL_PIXEL_SAMPLER;
	int v3 = c2->type == MTL_ARG_CODE_PIXEL_SAMPLER || c2->type == MTL_ARG_MATERIAL_PIXEL_SAMPLER;

	if (v4 == v3)
		return c1->dest - c2->dest;
  
	return v4 - v3;
}

SRCLINE(3994)
bool Material_AttemptCombineShaderArguments(MaterialShaderArgument *arg0, MaterialShaderArgument *arg1)
{
	if (arg0->type != arg1->type)
		return false;

	if (arg0->type != MTL_ARG_CODE_VERTEX_CONST && arg0->type != MTL_ARG_CODE_PIXEL_CONST)
		return false;

	if (arg0->u.codeConst.rowCount + arg0->dest != arg1->dest)
		return false;

	if (arg0->u.codeConst.index < R_MAX_CODE_INDEX)
		return false;

	if (arg0->u.codeConst.index != arg1->u.codeConst.index)
		return false;
						
	if (arg0->u.codeConst.rowCount + arg0->u.codeConst.firstRow != arg1->u.codeConst.firstRow)
		return false;

	ASSERT(!(arg1->u.codeConst.rowCount + arg0->u.codeConst.rowCount + arg0->u.codeConst.firstRow < 2
		|| arg1->u.codeConst.rowCount + arg0->u.codeConst.rowCount + arg0->u.codeConst.firstRow > 4));

	arg0->u.codeConst.rowCount += arg1->u.codeConst.rowCount;
	return true;
}

SRCLINE(4020)
unsigned int Material_CombineShaderArguments(unsigned int usedCount, MaterialShaderArgument *localArgs)
{
	int dstIndex = 0;

	for (unsigned int srcIndex = 1; srcIndex < usedCount; srcIndex++)
	{
		if (!Material_AttemptCombineShaderArguments(&localArgs[dstIndex], &localArgs[srcIndex]))
		{
			dstIndex++;
			
			localArgs[dstIndex].type			= localArgs[srcIndex].type;
			localArgs[dstIndex].dest			= localArgs[srcIndex].dest;
			localArgs[dstIndex].u.codeSampler	= localArgs[srcIndex].u.codeSampler;
		}
	}

	return dstIndex + 1;
}

SRCLINE(4037)
bool Material_SetShaderArguments(unsigned int usedCount, MaterialShaderArgument *localArgs, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args)
{
	ASSERT(args != nullptr);
	ASSERT(argCount != nullptr);

	if (usedCount)
	{
		if (*argCount + usedCount > argLimit)
		{
			Com_ScriptError("more than %i total shader arguments\n", argLimit);
			return false;
		}
		
		// Sort arguments so they can be easily combined
		qsort(localArgs, usedCount, sizeof(MaterialShaderArgument), Material_CompareShaderArgumentsForCombining);

		// Combine the arguments if possible (and get the new count)
		usedCount = Material_CombineShaderArguments(usedCount, localArgs);

		// Copy all of the fixed arguments to the real table
		memcpy(&args[*argCount], localArgs, sizeof(MaterialShaderArgument) * usedCount);

		// Adjust the output count
		*argCount += usedCount;
	}

	return true;
}

SRCLINE(4058)
ShaderUniformDef *Material_GetShaderArgumentDest(const char *paramName, unsigned int paramIndex, ShaderUniformDef *paramTable, unsigned int paramCount)
{
	for (unsigned int tableIndex = 0; tableIndex < paramCount; tableIndex++)
	{
		if (paramTable[tableIndex].index == paramIndex && !strcmp(paramTable[tableIndex].name, paramName))
		{
			if (paramTable[tableIndex].isAssigned)
			{
				Com_ScriptError("parameter %s index %i already assigned\n", paramName, paramIndex);
				return nullptr;
			}

			paramTable[tableIndex].isAssigned = true;
			return &paramTable[tableIndex];
		}
	}

	ASSERT_MSG(false, "unfound name should be caught earlier");
	return nullptr;
}

SRCLINE(4083)
bool MaterialAddShaderArgument(const char *shaderName, const char *paramName, MaterialShaderArgument *arg, char (*registerUsage)[64])
{
	if (arg->type > MTL_ARG_LITERAL_VERTEX_CONST && arg->type != MTL_ARG_CODE_VERTEX_CONST)
		return true;

	if (arg->dest >= R_MAX_PIXEL_SHADER_CONSTS)
	{
		Com_ScriptError("Invalid vertex register index %d in '%s' for '%s'\n", arg->dest, shaderName, paramName);
		return false;
	}

	if ((*registerUsage)[64 * arg->dest])
	{
		Com_ScriptError(
			"Vertex register collision at index %d in '%s' between '%s' and '%s'\n",
			arg->dest,
			shaderName,
			&(*registerUsage)[64 * arg->dest],
			paramName);

		return false;
	}

	I_strncpyz(&(*registerUsage)[64 * arg->dest], paramName, 64);
	return true;
}

SRCLINE(4112)
bool Material_AddShaderArgumentFromLiteral(const char *shaderName, const char *paramName, unsigned __int16 type, const float *literal, ShaderUniformDef *dest, MaterialShaderArgument *arg, char(*registerUsage)[64])
{
	ASSERT(!(type == MTL_ARG_LITERAL_PIXEL_CONST && arg->dest >= R_MAX_PIXEL_SHADER_CONSTS));

	arg->type			= type;
	arg->dest			= dest->resourceDest;
	arg->u.literalConst	= literal;

	return MaterialAddShaderArgument(shaderName, paramName, arg, registerUsage);
}

SRCLINE(4129)
bool Material_AddShaderArgumentFromCodeConst(const char *shaderName, const char *paramName, unsigned __int16 type, unsigned int codeIndex, unsigned int offset, ShaderUniformDef *dest, MaterialShaderArgument *arg, char(*registerUsage)[64])
{
	ASSERT(!(type == MTL_ARG_CODE_PIXEL_CONST && arg->dest >= R_MAX_PIXEL_SHADER_CONSTS));

	arg->type = type;
	arg->dest = dest->resourceDest;

	if (codeIndex < R_MAX_CODE_INDEX)
	{
		arg->u.codeConst.index		= offset + codeIndex;
		arg->u.codeConst.firstRow	= 0;
		arg->u.codeConst.rowCount	= 1;
	}
	else
	{
		if (dest->isTransposed)
			arg->u.codeConst.index = ((codeIndex - R_MAX_CODE_INDEX) ^ 2) + R_MAX_CODE_INDEX;
		else
			arg->u.codeConst.index = codeIndex;

		arg->u.codeConst.firstRow = offset;
		arg->u.codeConst.rowCount = 1;
	}

	return MaterialAddShaderArgument(shaderName, paramName, arg, registerUsage);
}

SRCLINE(4159)
void Material_AddShaderArgumentFromCodeSampler(unsigned __int16 type, unsigned int codeSampler, ShaderUniformDef *dest, MaterialShaderArgument *arg)
{
	arg->type			= type;
	arg->dest			= dest->resourceDest;
	arg->u.codeSampler	= codeSampler;
}

SRCLINE(4166)
bool Material_AddShaderArgumentFromMaterial(const char *shaderName, const char *paramName, unsigned __int16 type, const char *name, ShaderUniformDef *dest, MaterialShaderArgument *arg, char(*registerUsage)[64])
{
	ASSERT(!(type == MTL_ARG_MATERIAL_PIXEL_CONST && arg->dest >= R_MAX_PIXEL_SHADER_CONSTS));

	Material_RegisterString(name);

	arg->type			= type;
	arg->dest			= dest->resourceDest;
	arg->u.codeSampler	= R_HashString(name);

	return MaterialAddShaderArgument(shaderName, paramName, arg, registerUsage);
}

SRCLINE(4185)
bool Material_AddShaderArgument(const char *shaderName, ShaderArgumentSource *argSource, ShaderArgumentDest *argDest, ShaderUniformDef *paramTable, unsigned int paramCount, unsigned int *usedCount, MaterialShaderArgument *argTable, char(*registerUsage)[64])
{
	if (argSource->indexRange.isImplicit)
	{
		ASSERT(argSource->indexRange.first == 0);

		if (argDest->indexRange.count > argSource->indexRange.count)
		{
			Com_ScriptError("The destination needs %i entries, but the source can only provide %i",
				argDest->indexRange.count,
				argSource->indexRange.count);

			return false;
		}

		argSource->indexRange.count = argDest->indexRange.count;
	}
	else if (argDest->indexRange.count != argSource->indexRange.count)
	{
		Com_ScriptError("The destination needs %i entries, but the source provides %i",
			argDest->indexRange.count,
			argSource->indexRange.count);

		return false;
	}

	switch (argSource->type)
	{
	case MTL_ARG_LITERAL_VERTEX_CONST:
	case MTL_ARG_LITERAL_PIXEL_CONST:
	{
		if (argDest->indexRange.count != 1)
		{
			Com_ScriptError("Must assign literals to a constant one row at a time");
			return false;
		}

		ShaderUniformDef *dest = Material_GetShaderArgumentDest(
			argDest->paramName,
			argDest->indexRange.first,
			paramTable,
			paramCount);

		if (!dest)
			return false;

		if (!Material_AddShaderArgumentFromLiteral(
			shaderName,
			argDest->paramName,
			argSource->type,
			argSource->u.literalConst,
			dest,
			&argTable[*usedCount],
			registerUsage))
			return false;

		(*usedCount)++;
		return true;
	}

	case MTL_ARG_CODE_VERTEX_CONST:
	case MTL_ARG_CODE_PIXEL_CONST:
	{
		for (unsigned int indexOffset = 0; indexOffset < argDest->indexRange.count; indexOffset++)
		{
			ShaderUniformDef *dest = Material_GetShaderArgumentDest(
				argDest->paramName,
				indexOffset + argDest->indexRange.first,
				paramTable,
				paramCount);

			if (!dest)
				return false;

			if (!Material_AddShaderArgumentFromCodeConst(
				shaderName,
				argDest->paramName,
				argSource->type,
				argSource->u.codeIndex,
				indexOffset + argSource->indexRange.first,
				dest,
				&argTable[*usedCount],
				registerUsage))
				return false;

			(*usedCount)++;
		}

		return true;
	}

	case MTL_ARG_CODE_PIXEL_SAMPLER:
	{
		for (unsigned int indexOffset = 0; indexOffset < argDest->indexRange.count; indexOffset++)
		{
			ShaderUniformDef *dest = Material_GetShaderArgumentDest(
				argDest->paramName,
				indexOffset + argDest->indexRange.first,
				paramTable,
				paramCount);

			if (!dest)
				return false;

			Material_AddShaderArgumentFromCodeSampler(
				argSource->type,
				indexOffset + argSource->indexRange.first + argSource->u.codeIndex,
				dest,
				&argTable[*usedCount]);

			(*usedCount)++;
		}

		return true;
	}

	case MTL_ARG_MATERIAL_VERTEX_CONST:
	case MTL_ARG_MATERIAL_PIXEL_SAMPLER:
	case MTL_ARG_MATERIAL_PIXEL_CONST:
	{
		if (argDest->indexRange.count != 1)
		{
			Com_ScriptError("Must assign material values one at a time");
			return false;
		}

		ASSERT(argSource->indexRange.first == 0);
		ASSERT(argSource->indexRange.count == 1);

		ShaderUniformDef *dest = Material_GetShaderArgumentDest(
			argDest->paramName,
			argDest->indexRange.first,
			paramTable,
			paramCount);

		if (!dest)
			return false;

		if (!Material_AddShaderArgumentFromMaterial(
			shaderName,
			argDest->paramName,
			argSource->type,
			argSource->u.name,
			dest,
			&argTable[*usedCount],
			registerUsage))
			return false;

		(*usedCount)++;
		return true;
	}
	}

	ASSERT_MSG(false, "Unhandled case");
	return false;
}

SRCLINE(4275)
bool CodeConstIsOneOf(unsigned __int16 constCodeIndex, const unsigned __int16 *consts, int numConsts)
{
	for (int constIdx = 0; constIdx < numConsts; constIdx++)
	{
		if (consts[constIdx] == constCodeIndex)
			return true;
	}

	return false;
}

SRCLINE(4285)
bool Material_ParseShaderArguments(const char **text, const char *shaderName, MaterialShaderType shaderType, ShaderUniformDef *paramTable, unsigned int paramCount, unsigned __int16 *techFlags, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args)
{
	ASSERT(techFlags != nullptr);
	ASSERT(paramTable  != nullptr);

	if (!Material_MatchToken(text, "{"))
		return false;

	//
	// Values for shader registers stored as 64-byte strings
	//
	char registerUsage[R_MAX_PIXEL_SHADER_CONSTS][64];
	memset(registerUsage, 0, sizeof(registerUsage));

	//
	// Array of parsed shader arguments
	//
	MaterialShaderArgument localArgs[512];

	//
	// Current shader argument dest/source
	//
	ShaderArgumentDest argDest;
	ShaderArgumentSource argSource;

	unsigned int usedCount = 0;
	while (true)
	{
		const char *token = Com_Parse(text);

		if (!*token)
		{
			Com_ScriptError("unexpected end-of-file\n");
			return false;
		}

		if (*token == '}')
			break;

		char paramName[256];
		I_strncpyz(paramName, token, sizeof(paramName));

		ShaderParamType paramType;
		unsigned int registerCount = Material_ElemCountForParamName(shaderName, paramTable, paramCount, paramName, &paramType);

		if (registerCount)
		{
			if (!Material_ParseIndexRange(text, registerCount, &argDest.indexRange))
				return false;

			argDest.paramName = paramName;
			
			if (!Material_ParseArgumentSource(shaderType, text, shaderName, paramType, &argSource))
				return false;
			
			if (!Material_MatchToken(text, ";"))
				return false;
			
			if (!Material_AddShaderArgument(
				shaderName,
				&argSource,
				&argDest,
				paramTable,
				paramCount,
				&usedCount,
				localArgs,
				registerUsage))
				return false;

			if (argSource.type == MTL_ARG_CODE_PIXEL_SAMPLER)
			{
				switch (argSource.u.codeIndex)
				{
				case 9:
					*techFlags |= 1;
					break;
				case 10:
					*techFlags |= 2;
					break;
				case 19:
				case 20:
				case 21:
					*techFlags |= 64;
					break;
				}
			}
		}
		else
		{
			if (!Material_MatchToken(text, "="))
				return false;

			Com_SkipRestOfLine(text);
		}
	}

	if (usedCount == paramCount)
		return Material_SetShaderArguments(usedCount, localArgs, argLimit, argCount, args);

	//
	// Try and use default arguments for undefined parameters
	//
	for (unsigned int paramIndex = 0; paramIndex < paramCount; paramIndex++)
	{
		if (paramTable[paramIndex].isAssigned)
			continue;

		argDest.paramName				= paramTable[paramIndex].name;
		argDest.indexRange.first		= paramTable[paramIndex].index;
		argDest.indexRange.count		= 1;
		argDest.indexRange.isImplicit	= false;

		if (Material_DefaultArgumentSource(
			shaderType,
			paramTable[paramIndex].name,
			paramTable[paramIndex].type,
			&argDest.indexRange,
			&argSource))
		{
			printf("success\n");

			if (argSource.type == MTL_ARG_CODE_PIXEL_CONST)
			{
				if (argSource.u.codeIndex == 4)
					*techFlags |= 0x10u;
			}
			else if (argSource.type == MTL_ARG_CODE_PIXEL_SAMPLER)
			{
				if (argSource.u.codeIndex == 19 || argSource.u.codeIndex == 20 || argSource.u.codeIndex == 21)
					*techFlags |= 0x40u;
			}

			if (argSource.type == MTL_ARG_CODE_VERTEX_CONST && argSource.u.codeIndex == 120)
				*techFlags |= 0x100u;
			
			if (argSource.type == MTL_ARG_CODE_VERTEX_CONST)
			{
				if (!(*techFlags & 0x20))
				{
					static unsigned short foliageConsts[4] =
					{
						81,
						82,
						83,
						84
					};

					if (CodeConstIsOneOf(argSource.u.codeIndex, foliageConsts, 4))
						*techFlags |= 0x20u;
				}
			}

			if (argSource.type == MTL_ARG_CODE_PRIM_BEGIN && !(*techFlags & 0x200))
			{
				static unsigned short scorchConsts[2] =
				{
					114,
					118
				};

				if (CodeConstIsOneOf(argSource.u.codeIndex, scorchConsts, 2))
					*techFlags |= 0x200u;
			}

			if (!Material_AddShaderArgument(
				shaderName,
				&argSource,
				&argDest,
				paramTable,
				paramCount,
				&usedCount,
				localArgs,
				registerUsage))
				return false;
		}
	}

	//
	// Apply arguments if all parameters matched
	//
	if (usedCount == paramCount)
		return Material_SetShaderArguments(usedCount, localArgs, argLimit, argCount, args);

	//
	// Notify the user of any undefined arguments in the shader
	//
	Com_PrintWarning(8, "Undefined shader parameter(s) in %s\n", shaderName);

	for (unsigned int paramIndex = 0; paramIndex < paramCount; paramIndex++)
	{
		if (!paramTable[paramIndex].isAssigned)
			Com_PrintWarning(8, "  %s\n", paramTable[paramIndex].name);
	}

	Com_PrintWarning(8, "%i parameter(s) were undefined\n", paramCount - usedCount);
	return false;
}

SRCLINE(4720)
bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, LPD3DXBUFFER *shader)
{
	HRESULT hr = D3DXCreateBuffer(shaderLen, shader);

	if (!SUCCEEDED(hr))
	{
		Com_PrintError(8, "ERROR: Material_CopyTextToDXBuffer: D3DXCreateBuffer(%d) failed: %s (0x%08x)\n", shaderLen, R_ErrorDescription(hr), hr);

		free(cachedShader);
		return false;
	}

	memcpy((*shader)->GetBufferPointer(), cachedShader, shaderLen);
	return true;
}

FILE *Material_OpenShader_BlackOps(const char *shaderName, const char *shaderVersion)
{
	//
	// Determine if this was a vertex shader or pixel shader
	//
	const char *shaderMain;

	if (shaderVersion[0] == 'v' && shaderVersion[1] == 's')
		shaderMain = "vs_main";
	else
		shaderMain = "ps_main";

	//
	// Load the shader directly from the name
	//
	char shaderPath[MAX_PATH];
	Com_sprintf(shaderPath, MAX_PATH, "%s\\raw\\shadercache_mods\\%s_%s_%s",
		*(DWORD *)(*(DWORD *)0x258E9F0 + 12),
		shaderMain,
		shaderVersion,
		shaderName);

	return fopen(shaderPath, "rb");
}

FILE *Material_OpenShader_WAW(const char *shaderName, const char *shaderVersion)
{
	//
	// Create a unique shader string to hash
	//
	char shaderString[MAX_PATH];
	Com_sprintf(shaderString, MAX_PATH, "%s_%s", shaderVersion, shaderName);

	//
	// Determine the path to load the shader from
	//
	char shaderPath[MAX_PATH];
	Com_sprintf(shaderPath, MAX_PATH, "%s\\raw\\shader_bin\\%s_%8.8x",
		*(DWORD *)(*(DWORD *)0x258E9F0 + 12),
		shaderVersion,
		R_HashAssetName(shaderString));

	return fopen(shaderPath, "rb");
}

SRCLINE(4948)
void *Material_LoadShader(const char *shaderName, const char *shaderVersion)
{
	//
	// Try loading the black ops version first
	//
	int shaderDataSize	= 0;
	FILE *shaderFile	= Material_OpenShader_BlackOps(shaderName, shaderVersion);
	
	if (shaderFile)
	{
		// Skip the first 4 bytes (zeros)
		fpos_t pos = 4;
		fsetpos(shaderFile, &pos);

		// Read the real data size
		if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
		{
			fclose(shaderFile);
			return nullptr;
		}
	}
	else
	{
#if WAW_PIMP
		//
		// Load the WAW version if it wasn't found
		//
		shaderFile = Material_OpenShader_WAW(shaderName, shaderVersion);

		if (!shaderFile)
			return nullptr;

		if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
		{
			fclose(shaderFile);
			return nullptr;
		}
#else
		// WAW PIMP disabled and the shader wasn't found. Function has failed.
		return nullptr;
#endif
	}

	void *shaderMemory	= Z_Malloc(shaderDataSize);
	LPD3DXBUFFER shader	= nullptr;

	if (fread(shaderMemory, 1, shaderDataSize, shaderFile) != shaderDataSize)
		ASSERT_MSG(false, "Unable to read shader file data");

	if (!Material_CopyTextToDXBuffer(shaderMemory, shaderDataSize, &shader))
		ASSERT_MSG(false, "SHADER UPLOAD FAILED\n");

	fclose(shaderFile);
	Z_Free(shaderMemory);
	return shader;
}

SRCLINE(7491)
MaterialVertexShader *Material_RegisterVertexShader(const char *shaderName, char shaderVersion)
{
	static DWORD dwCall = 0x0052FBD0;

	__asm
	{
		movzx ecx, shaderVersion
		push ecx							// a3: shaderVersion

		mov ecx, shaderName					// a2: shaderName

		mov eax, dword ptr ds:[0x191D598]	// r_rendererInUse
		mov eax, dword ptr ds:[eax + 0xC]	// a1: renderer
		call [dwCall]
		add esp, 0x4
	}
}

SRCLINE(7569)
MaterialPixelShader *Material_RegisterPixelShader(const char *shaderName, char shaderVersion)
{
	static DWORD dwCall = 0x0052FCE0;

	__asm
	{
		movzx ecx, shaderVersion
		push ecx							// a3: shaderVersion

		mov ecx, shaderName					// a2: shaderName

		mov eax, dword ptr ds:[0x191D598]	// r_rendererInUse
		mov eax, dword ptr ds:[eax + 0xC]	// a1: renderer
		call [dwCall]
		add esp, 0x4
	}
}

SRCLINE(7604)
char Material_ParseShaderVersion(const char **text)
{
	float versionNumber = Com_ParseFloat(text);

	return (signed int)floor((versionNumber * 10.0f) + 0.5);
}

SRCLINE(7866)
char Material_GetStreamDestForSemantic(D3DXSEMANTIC *semantic)
{
#ifndef BO1_SEMI_NATIVE_BUILD
	return Material_GetStreamDestForSemantic_WAW(semantic);
#else
	switch (semantic->Usage)
	{
	case D3DDECLUSAGE_POSITION:
		if (semantic->UsageIndex)
			break;

		return 0;

	case D3DDECLUSAGE_BLENDWEIGHT:
		if (semantic->UsageIndex)
			break;

		return 19;

	case D3DDECLUSAGE_NORMAL:
		if (semantic->UsageIndex)
			break;

		return 1;

	case D3DDECLUSAGE_TEXCOORD:
		if (semantic->UsageIndex < 0 || semantic->UsageIndex >= 14)
			break;

		return semantic->UsageIndex + 5;

	case D3DDECLUSAGE_COLOR:
		if (semantic->UsageIndex < 0 || semantic->UsageIndex >= 2)
			break;

		return semantic->UsageIndex + 2;

	case D3DDECLUSAGE_DEPTH:
		if (semantic->UsageIndex)
			break;

		return 4;
	}

	Com_Error(ERR_DROP, "Unknown shader input/output usage %i:%i\n", semantic->Usage, semantic->UsageIndex);
	return 0;
#endif
}

SRCLINE(7929)
void Material_SetVaryingParameterDef(D3DXSEMANTIC *semantic, ShaderVaryingDef *paramDef)
{
	paramDef->streamDest	= Material_GetStreamDestForSemantic(semantic);
	paramDef->resourceDest	= paramDef->streamDest;
	paramDef->name			= Material_NameForStreamDest(paramDef->streamDest);
	paramDef->isAssigned	= false;
}

SRCLINE(7950)
bool Material_SetPassShaderArguments_DX(const char **text, const char *shaderName, MaterialShaderType shaderType, const DWORD *program, unsigned __int16 *techFlags, ShaderParameterSet *paramSet, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args)
{
	HRESULT hr = D3D_OK;

	//
	// Create a constant table from the shader program
	//
	LPD3DXCONSTANTTABLE constants;

	if (!SUCCEEDED(hr = D3DXGetShaderConstantTable(program, &constants)))
		goto __d3dfail;

	ASSERT(constants != nullptr);

	//
	// Get a pointer to the raw constant table
	//
	LPD3DXSHADER_CONSTANTTABLE constantTable	= (LPD3DXSHADER_CONSTANTTABLE)constants->GetBufferPointer();
	paramSet->uniformInputCount					= Material_PrepareToParseShaderArguments(constantTable, paramSet->uniformInputs);

	//
	// Parse all shader arguments
	//
	bool success = Material_ParseShaderArguments(
		text,
		shaderName,
		shaderType,
		paramSet->uniformInputs,
		paramSet->uniformInputCount,
		techFlags,
		argLimit,
		argCount,
		args);

	constants->Release();

	if (success)
	{
		//
		// Gather the input and output shader semantic tables
		//
		D3DXSEMANTIC inputSemantics[512];
		D3DXSEMANTIC outputSemantics[MAXD3DDECLLENGTH];

		UINT inputCount;
		if (!SUCCEEDED(hr = D3DXGetShaderInputSemantics(program, inputSemantics, &inputCount)))
			goto __d3dfail;

		UINT outputCount;
		if (!SUCCEEDED(hr = D3DXGetShaderOutputSemantics(program, outputSemantics, &outputCount)))
			goto __d3dfail;

		//
		// Set each varying parameter for inputs/outputs
		//
		paramSet->varyingInputCount = 0;

		for (UINT semanticIndex = 0; semanticIndex < inputCount; semanticIndex++)
			Material_SetVaryingParameterDef(&inputSemantics[semanticIndex], &paramSet->varyingInputs[paramSet->varyingInputCount++]);

		paramSet->outputCount = 0;

		for (UINT semanticIndex = 0; semanticIndex < outputCount; semanticIndex++)
		{
			if (outputSemantics[semanticIndex].Usage)
			{
				if (outputSemantics[semanticIndex].Usage != D3DDECLUSAGE_FOG)
					Material_SetVaryingParameterDef(&outputSemantics[semanticIndex], &paramSet->outputs[paramSet->outputCount++]);
			}
		}

		return true;
	}

	Com_ScriptError("Unable to parse shader arguments\n");
	return false;

__d3dfail:
	Com_ScriptError("Material_SetPassShaderArguments_DX D3D Error: %s (%08x)\n", R_ErrorDescription(hr), hr);
	return false;
}

SRCLINE(8005)
bool Material_LoadPassVertexShader(const char **text, unsigned __int16 *techFlags, ShaderParameterSet *paramSet, MaterialPass *pass, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args)
{
	memset(paramSet, 0, sizeof(ShaderParameterSet));

	if (!Material_MatchToken(text, "vertexShader"))
		return false;

	char shaderVersion					= Material_ParseShaderVersion(text);
	const char *shaderName				= Com_Parse(text);
	MaterialVertexShader *mtlShader		= Material_RegisterVertexShader(shaderName, shaderVersion);

	if (!mtlShader)
		return false;

	pass->vertexShader = mtlShader;

	return Material_SetPassShaderArguments_DX(
		text,
		mtlShader->name,
		MTL_VERTEX_SHADER,
		mtlShader->prog.loadDef.program,
		techFlags,
		paramSet,
		argLimit,
		argCount,
		args);
}

SRCLINE(8049)
bool Material_LoadDeclTypes(const char **text, MaterialPass *pass)
{
	ASSERT(pass->vertexShader != nullptr);

	if (!strcmp(Com_Parse(text), "declTypes"))
	{
		while (strcmp("}", Com_Parse(text)))
			/* Nothing */;
	}
	else
		Com_UngetToken();

	return true;
}

SRCLINE(8132)
bool Material_LoadPassPixelShader(const char **text, unsigned __int16 *techFlags, ShaderParameterSet *paramSet, MaterialPass *pass, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args)
{
	memset(paramSet, 0, sizeof(ShaderParameterSet));

	if (!Material_MatchToken(text, "pixelShader"))
		return false;

	char shaderVersion				= Material_ParseShaderVersion(text);
	const char *shaderName			= Com_Parse(text);
	MaterialPixelShader *mtlShader	= Material_RegisterPixelShader(shaderName, shaderVersion);

	if (!mtlShader)
		return false;

	pass->pixelShader = mtlShader;

	return Material_SetPassShaderArguments_DX(
		text,
		mtlShader->name,
		MTL_PIXEL_SHADER,
		mtlShader->prog.loadDef.program,
		techFlags,
		paramSet,
		argLimit,
		argCount,
		args);
}

SRCLINE(8181)
bool Material_HasMatchingParameter(char find, ShaderVaryingDef *paramTable, unsigned int paramCount)
{
	for (unsigned int paramIndex = 0; paramIndex < paramCount; paramIndex++)
	{
		if (paramTable[paramIndex].resourceDest == find)
			return true;
	}

	return false;
}

SRCLINE(8197)
bool Material_HasMatchingParameter_BuggySdkWorkaround(char find, ShaderVaryingDef *paramTable, unsigned int paramCount)
{
	if (find == 2)
		return Material_HasMatchingParameter(3, paramTable, paramCount);
	else if (find == 3)
		return Material_HasMatchingParameter(2, paramTable, paramCount);

	return false;
}

SRCLINE(8211)
bool Material_ValidateShaderLinkage(ShaderVaryingDef *vertexOutputs, unsigned int vertexOutputCount, ShaderVaryingDef *pixelInputs, unsigned int pixelInputCount)
{
	bool isValid = true;

	for (unsigned int paramIndex = 0; paramIndex < pixelInputCount; paramIndex++)
	{
		if (!Material_HasMatchingParameter(pixelInputs[paramIndex].resourceDest, vertexOutputs, vertexOutputCount)
			&& !Material_HasMatchingParameter_BuggySdkWorkaround(pixelInputs[paramIndex].resourceDest, vertexOutputs, vertexOutputCount))
		{
			Com_ScriptError(
				"Pixel shader input '%s' doesn't have a corresponding vertex shader output.\n",
				pixelInputs[paramIndex].name);

			isValid = false;
		}
	}

	for (unsigned int paramIndex = 0; paramIndex < vertexOutputCount; paramIndex++)
	{
		if (!Material_HasMatchingParameter(vertexOutputs[paramIndex].resourceDest, pixelInputs, pixelInputCount))
			Material_HasMatchingParameter_BuggySdkWorkaround(vertexOutputs[paramIndex].resourceDest, pixelInputs, pixelInputCount);
	}

	return isValid;
}

SRCLINE(8262)
MaterialUpdateFrequency Material_GetArgUpdateFrequency(MaterialShaderArgument *arg)
{
	ASSERT(arg != nullptr);

	MaterialUpdateFrequency updateFreq;
	switch (arg->type)
	{
	case MTL_ARG_CODE_VERTEX_CONST:
		updateFreq = s_codeConstUpdateFreq[arg->u.codeConst.index];
		break;

	case MTL_ARG_CODE_PIXEL_SAMPLER:
		updateFreq = s_codeSamplerUpdateFreq[arg->u.codeSampler];

		ASSERT((updateFreq == MTL_UPDATE_PER_OBJECT) || (updateFreq == MTL_UPDATE_RARELY) || (updateFreq == MTL_UPDATE_CUSTOM));
		break;

	case MTL_ARG_CODE_PIXEL_CONST:
		updateFreq = s_codeConstUpdateFreq[arg->u.codeConst.index];

		//ASSERT(updateFreq == MTL_UPDATE_RARELY); TODO
		break;

	default:
		updateFreq = MTL_UPDATE_RARELY;
		break;
	}

	return updateFreq;
}


SRCLINE(8288)
int Material_CompareShaderArgumentsForRuntime(const void *e0, const void *e1)
{
	MaterialShaderArgument *c1 = (MaterialShaderArgument *)e0;
	MaterialShaderArgument *c2 = (MaterialShaderArgument *)e1;

	MaterialUpdateFrequency updateFreq1 = Material_GetArgUpdateFrequency(c1);
	MaterialUpdateFrequency updateFreq2 = Material_GetArgUpdateFrequency(c2);

	if (updateFreq1 == updateFreq2)
	{
		if (c1->type == c2->type)
		{
			if (c1->type && c1->type != MTL_ARG_MATERIAL_PIXEL_CONST && c1->type != MTL_ARG_MATERIAL_PIXEL_SAMPLER)
				return c1->dest - c2->dest;

			return c1->u.codeSampler < c2->u.codeSampler ? -1 : 1;
		}

		return c1->type - c2->type;
	}

	return updateFreq1 - updateFreq2;
}

SRCLINE(8306)
char Material_CountArgsWithUpdateFrequency(MaterialUpdateFrequency updateFreq, MaterialShaderArgument *args, unsigned int argCount, unsigned int *firstArg)
{
	args		= &args[*firstArg];
	argCount	= argCount - *firstArg;

	unsigned int matchCount = 0;
	for (; matchCount < argCount && Material_GetArgUpdateFrequency(&args[matchCount]) == updateFreq; matchCount++)
		/* Do nothing */;

	*firstArg += matchCount;
	return matchCount;
}

SRCLINE(8375)
bool Material_LoadPass(const char **text, unsigned __int16 *techFlags, MaterialPass *pass, MaterialStateMap **stateMap, int rendererInUse)
{
	memset(pass, 0, sizeof(MaterialPass));

	//
	// State map
	//
	if (!Material_LoadPassStateMap(text, stateMap))
		return false;

	//
	// Shader argument array instance
	//
	unsigned int argCount = 0;
	MaterialShaderArgument args[128];

	//
	// Load the vertex shader pass
	//
	ShaderParameterSet vertexParamSet;

	if (!Material_LoadPassVertexShader(text, techFlags, &vertexParamSet, pass, ARRAYSIZE(args), &argCount, args))
		return false;

	if (argCount <= 0)
	{
		Com_ScriptError("material has no vertex arguments; it should at least have a transform matrix.\n");
		return false;
	}

	//
	// Load the pixel shader pass
	//
	ShaderParameterSet pixelParamSet;

	if (!Material_LoadPassPixelShader(text, techFlags, &pixelParamSet, pass, ARRAYSIZE(args), &argCount, args))
		return false;

	//
	// Sort arguments based on update frequency
	//
	qsort(args, argCount, sizeof(MaterialShaderArgument), Material_CompareShaderArgumentsForRuntime);

	//
	// Parse default material update frequency counts
	//
	unsigned int firstArg	= 0;
	pass->perPrimArgCount	= Material_CountArgsWithUpdateFrequency(MTL_UPDATE_PER_PRIM, args, argCount, &firstArg);
	pass->perObjArgCount	= Material_CountArgsWithUpdateFrequency(MTL_UPDATE_PER_OBJECT, args, argCount, &firstArg);
	pass->stableArgCount	= Material_CountArgsWithUpdateFrequency(MTL_UPDATE_RARELY, args, argCount, &firstArg);

	//
	// Parse custom material frequency arguments
	//
	{
		MaterialShaderArgument *customArg	= &args[firstArg];
		unsigned int customCount			= Material_CountArgsWithUpdateFrequency(MTL_UPDATE_CUSTOM, args, argCount, &firstArg);
		unsigned int customArgIndex			= 0;
		pass->customSamplerFlags			= 0;

		while (customArgIndex < customCount)
		{
			ASSERT(customArg->type == MTL_ARG_CODE_PIXEL_SAMPLER);
			ASSERT(customArg->dest != SAMPLER_INDEX_INVALID);

			int customSamplerIndex = 0;
			for (; customSamplerIndex < CUSTOM_SAMPLER_COUNT; customSamplerIndex++)
			{
				if (customArg->u.codeSampler == g_customSamplerSrc[customSamplerIndex])
				{
					ASSERT(!(pass->customSamplerFlags & (1 << customSamplerIndex)));
					//ASSERT(customArg->dest == g_customSamplerDest[customSamplerIndex]); TODO

					pass->customSamplerFlags |= 1 << customSamplerIndex;
					break;
				}
			}

			ASSERT(customSamplerIndex != CUSTOM_SAMPLER_COUNT);

			customArgIndex++;
			customArg++;
		}
	}

	//
	// Allocate space for the local arguments
	//
	argCount		= pass->stableArgCount + pass->perObjArgCount + pass->perPrimArgCount;
	pass->localArgs = (MaterialShaderArgument *)Material_Alloc(sizeof(MaterialShaderArgument) * argCount);

	//
	// Copy the data over to the pointer
	//
	memcpy(pass->localArgs, args, sizeof(MaterialShaderArgument) * argCount);

	//
	// DEBUG: Test/assert argument type indexes
	//
	MaterialShaderArgument *arg = pass->localArgs;
	int argIndex				= 0;

	while (argIndex < pass->perPrimArgCount)
	{
		ASSERT(arg->type >= MTL_ARG_CODE_PRIM_BEGIN);
		ASSERT(arg->type < MTL_ARG_CODE_PRIM_END);

		argIndex++;
		arg++;
	}

	arg			= &pass->localArgs[pass->perPrimArgCount];
	argIndex	= 0;

	while (argIndex < pass->perObjArgCount)
	{
		ASSERT(arg->type >= MTL_ARG_CODE_PRIM_BEGIN);
		ASSERT(arg->type < MTL_ARG_CODE_PRIM_END);

		argIndex++;
		arg++;
	}

	//
	// Validate all shader parameters
	//
	if (Material_ValidateShaderLinkage(
		vertexParamSet.outputs,
		vertexParamSet.outputCount,
		pixelParamSet.varyingInputs,
		pixelParamSet.varyingInputCount))
	{
		bool success = Material_LoadPassVertexDecl(
			text,
			vertexParamSet.varyingInputs,
			vertexParamSet.varyingInputCount,
			pass);

		Material_LoadDeclTypes(text, pass);

		if (strstr(*text, "vertexDef"))
		{
			while (strcmp(";", Com_Parse(text)))
				/* Nothing */;
		}

		return success;
	}

	return false;
}

SRCLINE(8710)
void *Material_RegisterTechnique(const char *name, int renderer)
{
	static DWORD dwCall = 0x00530A40;

	__asm
	{
		mov esi, renderer
		mov eax, name
		call [dwCall]
	}
}

SRCLINE(8727)
bool Material_IgnoreTechnique(const char *name)
{
	const char *techniqueNames[1];
	techniqueNames[0] = "\"none\"";

	for (int techniqueIndex = 0; techniqueIndex < 1; techniqueIndex++)
	{
		if (!strcmp(name, techniqueNames[techniqueIndex]))
			return true;
	}

	return false;
}

SRCLINE(8763)
int Material_TechniqueTypeForName(const char *name)
{
//#ifndef BO1_BUILD
	return Material_TechniqueTypeForName_WAW(name);
//#else
#if 0
	const char *techniqueNames[130];
	techniqueNames[0] = "\"depth prepass\"";
	techniqueNames[1] = "\"build floatz\"";
	techniqueNames[2] = "\"build shadowmap depth\"";
	techniqueNames[3] = "\"build shadowmap color\"";
	techniqueNames[4] = "\"unlit\"";
	techniqueNames[5] = "\"emissive\"";
	techniqueNames[6] = "\"emissive shadow\"";
	techniqueNames[7] = "\"emissive nvintz\"";
	techniqueNames[8] = "\"emissive shadow nvintz\"";
	techniqueNames[9] = "\"emissive reflected\"";
	techniqueNames[10] = "\"lit\"";
	techniqueNames[11] = "\"lit sun\"";
	techniqueNames[12] = "\"lit sun shadow\"";
	techniqueNames[13] = "\"lit spot\"";
	techniqueNames[14] = "\"lit spot shadow\"";
	techniqueNames[15] = "\"lit omni\"";
	techniqueNames[16] = "\"lit omni shadow\"";
	techniqueNames[17] = "\"lit dlight\"";
	techniqueNames[18] = "\"lit sun dlight\"";
	techniqueNames[19] = "\"lit sun shadow dlight\"";
	techniqueNames[20] = "\"lit spot dlight\"";
	techniqueNames[21] = "\"lit spot shadow dlight\"";
	techniqueNames[22] = "\"lit omni dlight\"";
	techniqueNames[23] = "\"lit omni shadow dlight\"";
	techniqueNames[24] = "\"lit glight\"";
	techniqueNames[25] = "\"lit sun glight\"";
	techniqueNames[26] = "\"lit sun shadow glight\"";
	techniqueNames[27] = "\"lit spot glight\"";
	techniqueNames[28] = "\"lit spot shadow glight\"";
	techniqueNames[29] = "\"lit omni glight\"";
	techniqueNames[30] = "\"lit omni shadow glight\"";
	techniqueNames[31] = "\"lit dlight glight\"";
	techniqueNames[32] = "\"lit sun dlight glight\"";
	techniqueNames[33] = "\"lit sun shadow dlight glight\"";
	techniqueNames[34] = "\"lit spot dlight glight\"";
	techniqueNames[35] = "\"lit spot shadow dlight glight\"";
	techniqueNames[36] = "\"lit omni dlight glight\"";
	techniqueNames[37] = "\"lit omni shadow dlight glight\"";
	techniqueNames[38] = "\"lit alpha\"";
	techniqueNames[39] = "\"lit sun alpha\"";
	techniqueNames[40] = "\"lit sun shadow alpha\"";
	techniqueNames[41] = "\"lit spot alpha\"";
	techniqueNames[42] = "\"lit spot shadow alpha\"";
	techniqueNames[43] = "\"lit omni alpha\"";
	techniqueNames[44] = "\"lit omni shadow alpha\"";
	techniqueNames[45] = "\"lit remap\"";
	techniqueNames[46] = "\"lit sun remap\"";
	techniqueNames[47] = "\"lit sun shadow remap\"";
	techniqueNames[48] = "\"lit spot remap\"";
	techniqueNames[49] = "\"lit spot shadow remap\"";
	techniqueNames[50] = "\"lit omni remap\"";
	techniqueNames[51] = "\"lit omni shadow remap\"";
	techniqueNames[52] = "\"lit fade\"";
	techniqueNames[53] = "\"lit sun fade\"";
	techniqueNames[54] = "\"lit sun shadow fade\"";
	techniqueNames[55] = "\"lit spot fade\"";
	techniqueNames[56] = "\"lit spot shadow fade\"";
	techniqueNames[57] = "\"lit omni fade\"";
	techniqueNames[58] = "\"lit omni shadow fade\"";
	techniqueNames[59] = "\"lit charred\"";
	techniqueNames[60] = "\"lit fade charred\"";
	techniqueNames[61] = "\"lit sun charred\"";
	techniqueNames[62] = "\"lit sun fade charred\"";
	techniqueNames[63] = "\"lit sun shadow charred\"";
	techniqueNames[64] = "\"lit sun shadow fade charred\"";
	techniqueNames[65] = "\"lit spot charred\"";
	techniqueNames[66] = "\"lit spot fade charred\"";
	techniqueNames[67] = "\"lit spot shadow charred\"";
	techniqueNames[68] = "\"lit spot shadow fade charred\"";
	techniqueNames[69] = "\"lit omni charred\"";
	techniqueNames[70] = "\"lit omni fade charred\"";
	techniqueNames[71] = "\"lit omni shadow charred\"";
	techniqueNames[72] = "\"lit omni shadow fade charred\"";
	techniqueNames[73] = "\"lit instanced\"";
	techniqueNames[74] = "\"lit instanced sun\"";
	techniqueNames[75] = "\"lit instanced sun shadow\"";
	techniqueNames[76] = "\"lit instanced spot\"";
	techniqueNames[77] = "\"lit instanced spot shadow\"";
	techniqueNames[78] = "\"lit instanced omni\"";
	techniqueNames[79] = "\"lit instanced omni shadow\"";
	techniqueNames[80] = "\"lit nvintz\"";
	techniqueNames[81] = "\"lit sun nvintz\"";
	techniqueNames[82] = "\"lit sun shadow nvintz\"";
	techniqueNames[83] = "\"lit spot nvintz\"";
	techniqueNames[84] = "\"lit spot shadow nvintz\"";
	techniqueNames[85] = "\"lit omni nvintz\"";
	techniqueNames[86] = "\"lit omni shadow nvintz\"";
	techniqueNames[87] = "\"lit dlight nvintz\"";
	techniqueNames[88] = "\"lit sun dlight nvintz\"";
	techniqueNames[89] = "\"lit sun shadow dlight nvintz\"";
	techniqueNames[90] = "\"lit spot dlight nvintz\"";
	techniqueNames[91] = "\"lit spot shadow dlight nvintz\"";
	techniqueNames[92] = "\"lit omni dlight nvintz\"";
	techniqueNames[93] = "\"lit omni shadow dlight nvintz\"";
	techniqueNames[94] = "\"lit glight nvintz\"";
	techniqueNames[95] = "\"lit sun glight nvintz\"";
	techniqueNames[96] = "\"lit sun shadow glight nvintz\"";
	techniqueNames[97] = "\"lit spot glight nvintz\"";
	techniqueNames[98] = "\"lit spot shadow glight nvintz\"";
	techniqueNames[99] = "\"lit omni glight nvintz\"";
	techniqueNames[100] = "\"lit omni shadow glight nvintz\"";
	techniqueNames[101] = "\"lit dlight glight nvintz\"";
	techniqueNames[102] = "\"lit sun dlight glight nvintz\"";
	techniqueNames[103] = "\"lit sun shadow dlight glight nvintz\"";
	techniqueNames[104] = "\"lit spot dlight glight nvintz\"";
	techniqueNames[105] = "\"lit spot shadow dlight glight nvintz\"";
	techniqueNames[106] = "\"lit omni dlight glight nvintz\"";
	techniqueNames[107] = "\"lit omni shadow dlight glight nvintz\"";
	techniqueNames[108] = "\"lit instanced nvintz\"";
	techniqueNames[109] = "\"lit instanced sun nvintz\"";
	techniqueNames[110] = "\"lit instanced sun shadow nvintz\"";
	techniqueNames[111] = "\"lit instanced spot nvintz\"";
	techniqueNames[112] = "\"lit instanced spot shadow nvintz\"";
	techniqueNames[113] = "\"lit instanced omni nvintz\"";
	techniqueNames[114] = "\"lit instanced omni shadow nvintz\"";
	techniqueNames[115] = "\"light spot\"";
	techniqueNames[116] = "\"light omni\"";
	techniqueNames[117] = "\"light spot shadow\"";
	techniqueNames[118] = "\"light spot charred\"";
	techniqueNames[119] = "\"light omni charred\"";
	techniqueNames[120] = "\"light spot shadow charred\"";
	techniqueNames[121] = "\"fakelight normal\"";
	techniqueNames[122] = "\"fakelight view\"";
	techniqueNames[123] = "\"sunlight preview\"";
	techniqueNames[124] = "\"case texture\"";
	techniqueNames[125] = "\"solid wireframe\"";
	techniqueNames[126] = "\"shaded wireframe\"";
	techniqueNames[127] = "\"debug bumpmap\"";
	techniqueNames[128] = "\"debug bumpmap instanced\"";
	techniqueNames[129] = "\"impact mask\"";

	for (unsigned int techniqueIndex = 0; techniqueIndex < 130; ++techniqueIndex)
	{
		if (!strcmp(name, techniqueNames[techniqueIndex]))
			return techniqueIndex;
	}

	return 130;
#endif
}

SRCLINE(9023)
void *__cdecl Material_LoadTechniqueSet(const char *name, int renderer)
{
	//
	// Create a file path using normal techsets and read data
	//
	char filename[MAX_PATH];
#if WAW_PIMP
	Com_sprintf(filename, MAX_PATH, "waw_pimp/techsets/%s.techset", name);
#else
	Com_sprintf(filename, MAX_PATH, "techsets/%s.techset", name);
#endif

	void *fileData;
	int fileSize = FS_ReadFile(filename, (void **)&fileData);

	if (fileSize < 0)
	{
		//
		// Try loading with PIMP enabled
		//
		//Com_sprintf(filename, MAX_PATH, "pimp/techsets/%s.techset", name);
		//fileSize = FS_ReadFile(filename, (void **)&fileData);

		if (fileSize < 0)
		{
			if (strcmp(name, "default") != 0)
			{
				void* result = Material_LoadTechniqueSet("default", renderer);
				if (!result)
					Com_PrintError(8, "^1ERROR: Couldn't override techniqueSet '%s' (\'default\' techniqueSet is missing)\n", filename);
				return result;
			}
			else
			{
				Com_PrintError(8, "^1ERROR: Couldn't open techniqueSet '%s'\n", filename);
				return nullptr;
			}
		}
	}

	//
	// Allocate the techset structure
	//
	const char *textData = (const char *)fileData;
	size_t nameSize = strlen(name) + 1;
	char *techniqueSet = (char *)Z_Malloc(nameSize + 249);

	*(char **)(techniqueSet + 0) = techniqueSet + 248;
	*(BYTE *)(techniqueSet + 4) = 0;
	*(char **)(techniqueSet + 8) = techniqueSet;

	memcpy(techniqueSet + 248, name, nameSize);

	//
	// Begin the text parsing session
	//
	// TODO: What does this function actually do?
	((void(__cdecl *)())0x005525D0)();
	Com_BeginParseSession(filename);
	Com_SetScriptWarningPrefix("^1ERROR: ");
	Com_SetSpaceDelimited(0);
	Com_SetKeepStringQuotes(1);

	char techType[TECH_TYPE_COUNT];
	int techTypeCount	= 0;
	bool usingTechnique = false;
	while (true)
	{
		const char *token = Com_Parse(&textData);

		if (*token == '\0')
			break;

		if (*token == '"')
		{
			if (techTypeCount == TECH_TYPE_COUNT)
			{
				Com_ScriptError("Too many labels in technique set\n");
				techniqueSet = 0;
				break;
			}

			if (!Material_IgnoreTechnique(token))
			{
				techType[techTypeCount] = Material_TechniqueTypeForName(token);

				if (techType[techTypeCount] == TECH_TYPE_COUNT)
				{
					Com_ScriptError("Unknown technique type '%s'\n", token);
					techniqueSet = 0;
					break;
				}

				if (Material_UsingTechnique(techType[techTypeCount]))
					usingTechnique = true;

				techTypeCount++;
			}

			if (!Material_MatchToken(&textData, ":"))
			{
				techniqueSet = 0;
				break;
			}
		}
		else
		{
			if (usingTechnique)
			{
				if (!techTypeCount)
				{
					Com_ScriptError("Unknown technique type '%s'\n", token);
					techniqueSet = 0;
					break;
				}

				void *technique = Material_RegisterTechnique(token, renderer);
				if (!technique)
				{
					Com_ScriptError("Couldn't register technique '%s'\n", token);
					techniqueSet = 0;
					break;
				}

				for (int techTypeIndex = 0; techTypeIndex < techTypeCount; techTypeIndex++)
					*(DWORD *)&techniqueSet[4 * techType[techTypeIndex] + 12] = (DWORD)technique;
			}

			techTypeCount = 0;
			usingTechnique = false;
			if (!Material_MatchToken(&textData, ";"))
			{
				techniqueSet = 0;
				break;
			}
		}
	}

	Com_EndParseSession();
	FS_FreeFile(fileData);
	return techniqueSet;
}

void __declspec(naked) hk_Material_LoadShader()
{
	__asm
	{
		push ebp
		mov ebp, esp

		push ecx
		push [ebp + 0x8]
		call Material_LoadShader
		add esp, 0x8

		pop ebp
		retn
	}
}

void __declspec(naked) hk_Material_LoadPass()
{
	__asm
	{
		push ebp
		mov ebp, esp

		push [ebp + 0xC]	// a5: rendererInUse
		push [ebp + 0x14]	// a4: stateMap
		push [ebp + 0x10]	// a3: pass
		push ecx			// a2: techFlags
		push [ebp + 0x8]	// a1: text
		call Material_LoadPass
		add esp, 0x14

		pop ebp
		retn
	}
}

stream_source_info_t s_streamSourceInfo[18][10] =
{
	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 1 },
		{ 0, 28, 5 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 5 },
		{ 0, 24, 5 },
		{ 0, 28, 5 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 1 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 1 },
		{ -1, 0, 0 },
		{ 1, 8, 8 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ -1, 0, 0 },
		{ 1, 16, 8 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ -1, 0, 0 },
		{ 1, 16, 8 },
		{ 1, 20, 8 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ 1, 16, 1 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ 1, 16, 1 },
		{ 1, 24, 8 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ 1, 16, 1 },
		{ 1, 24, 8 },
		{ 1, 28, 8 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ 1, 16, 3 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ 1, 16, 3 },
		{ 1, 32, 8 },
		{ -1, 0, 0 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ 0, 16, 4 },
		{ 0, 20, 3 },
		{ 0, 36, 5 },
		{ 0, 40, 5 },
		{ 1, 0, 3 },
		{ 1, 16, 3 },
		{ 1, 32, 8 },
		{ 1, 36, 8 },
		{ 2, 0, 15 },
	},

	{
		{ 0, 0, 3 },
		{ -1, 0, 0 },
		{ 0, 12, 1 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
	},

	{
		{ 0, 0, 2 },
		{ 0, 12, 4 },
		{ 0, 16, 5 },
		{ 0, 20, 5 },
		{ 0, 24, 5 },
		{ 0, 28, 5 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
	},

	{
		{ 0, 0, 5 },
		{ 0, 8, 4 },
		{ 0, 12, 5 },
		{ 0, 4, 5 },
		{ -1, 0, 0 },
		{ 1, 0, 5 },
		{ -1, 0, 0 },
		{ 1, 4, 5 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
	},

	{
		{ 0, 0, 10 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },
	},
};

stream_dest_info_t s_streamDestInfo[20] =
{
	{ 0, 0 },
	{ 3, 0 },
	{ 10, 0 },
	{ 10, 1 },
	{ 12, 0 },
	{ 5, 0 },
	{ 5, 1 },
	{ 5, 2 },
	{ 5, 3 },
	{ 5, 4 },
	{ 5, 5 },
	{ 5, 6 },
	{ 5, 7 },
	{ 5, 8 },
	{ 5, 9 },
	{ 5, 10 },
	{ 5, 11 },
	{ 5, 12 },
	{ 5, 13 },
	{ 1, 0 },
};

unsigned int g_customSamplerSrc[4] =
{
	27, 4, 5, 33,
};

unsigned int g_customSamplerDest[4] =
{
	15, 12, 13, 14,
};

const MaterialUpdateFrequency s_codeConstUpdateFreq[197] =
{
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 1, 1, 1, 1, 1, 1, 0, 1,
	0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2,
};

const MaterialUpdateFrequency s_codeSamplerUpdateFreq[43] =
{
	2, 2, 2, 2, 3, 3, 2, 2, 1, 2,
	2, 2, 2, 2, 2, 2, 1, 2, 2, 2,
	2, 2, 1, 1, 1, 1, 1, 3, 2, 1,
	1, 1, 1, 3, 2, 2, 2, 2, 2, 2,
	2, 1, 2,
};

const bool g_useTechnique[130] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1,
};

CodeSamplerSource s_lightmapSamplers[] =
{
	{ "primary", 4, 0, 0, 0 },
	{ "secondary", 5, 0, 0, 0 },
	{ "secondaryb", 33, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeSamplerSource s_lightSamplers[] =
{
	{ "attenuation", 16, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeSamplerSource s_codeSamplers[] =
{
	{ "white", 1, 0, 0, 0 },
	{ "black", 0, 0, 0, 0 },
	{ "identityNormalMap", 2, 0, 0, 0 },
	{ "lightmap", 4, s_lightmapSamplers, 0, 0 },
	{ "outdoor", 18, 0, 0, 0 },
	{ "shadowmapSun", 6, 0, 0, 0 },
	{ "shadowmapSpot", 7, 0, 0, 0 },
	{ "feedback", 8, 0, 0, 0 },
	{ "resolvedPostSun", 9, 0, 0, 0 },
	{ "resolvedScene", 10, 0, 0, 0 },
	{ "postEffectSrc", 11, 0, 0, 0 },
	{ "postEffectGodRays", 12, 0, 0, 0 },
	{ "postEffect0", 13, 0, 0, 0 },
	{ "postEffect1", 14, 0, 0, 0 },
	{ "sky", 15, 0, 0, 0 },
	{ "light", 16, s_lightSamplers, 0, 0 },
	{ "floatZ", 19, 0, 0, 0 },
	{ "processedFloatZ", 20, 0, 0, 0 },
	{ "rawFloatZ", 21, 0, 0, 0 },

	{ "caseTexture", 22, 0, 0, 0 }, // from linker_pc

	{ "codeTexture0", 34, 0, 0, 0 },
	{ "codeTexture1", 35, 0, 0, 0 },
	{ "codeTexture2", 36, 0, 0, 0 },
	{ "codeTexture3", 37, 0, 0, 0 },
	{ "impactMask", 38, 0, 0, 0 },
	{ "ui3d", 39, 0, 0, 0 },
	{ "missileCam", 40, 0, 0, 0 },
	{ "compositeResult", 41, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeSamplerSource s_defaultCodeSamplers[] =
{
	{ "shadowmapSamplerSun", 6, 0, 0, 0 },
	{ "shadowmapSamplerSpot", 7, 0, 0, 0 },
	{ "feedbackSampler", 8, 0, 0, 0 },
	{ "floatZSampler", 19, 0, 0, 0 },
	{ "processedFloatZSampler", 20, 0, 0, 0 },
	{ "rawFloatZSampler", 21, 0, 0, 0 },
	{ "featherFloatZSampler", 28, 0, 0, 0 },
	{ "attenuationSampler", 16, 0, 0, 0 },
	{ "dlightAttenuationSampler", 17, 0, 0, 0 },
	{ "lightmapSamplerPrimary", 4, 0, 0, 0 },
	{ "lightmapSamplerSecondary", 5, 0, 0, 0 },
	{ "lightmapSamplerSecondaryB", 33, 0, 0, 0 },
	{ "modelLightingSampler", 3, 0, 0, 0 },
	{ "cinematicYSampler", 23, 0, 0, 0 },
	{ "cinematicCrSampler", 24, 0, 0, 0 },
	{ "cinematicCbSampler", 25, 0, 0, 0 },
	{ "cinematicASampler", 26, 0, 0, 0 },
	{ "reflectionProbeSampler", 27, 0, 0, 0 },
	{ "terrainScorchTextureSampler0", 29, 0, 0, 0 },
	{ "terrainScorchTextureSampler1", 30, 0, 0, 0 },
	{ "terrainScorchTextureSampler2", 31, 0, 0, 0 },
	{ "terrainScorchTextureSampler3", 32, 0, 0, 0 },
	{ "impactMaskSampler", 38, 0, 0, 0 },
	{ "ui3dSampler", 39, 0, 0, 0 },
	{ "missileCamSampler", 40, 0, 0, 0 },
	{ "heatmapSampler", 42, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeConstantSource s_nearPlaneConsts[] =
{
	{ "org", 16, 0, 0, 0 },
	{ "dx", 17, 0, 0, 0 },
	{ "dy", 18, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeConstantSource s_sunConsts[] =
{
	{ "position", 50, 0, 0, 0 },
	{ "diffuse", 51, 0, 0, 0 },
	{ "specular", 52, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeConstantSource s_lightConsts[] =
{
	{ "position", 0, 0, 0, 0 },
	{ "diffuse", 1, 0, 0, 0 },
	{ "specular", 2, 0, 0, 0 },
	{ "spotDir", 3, 0, 0, 0 },
	{ "spotFactors", 4, 0, 0, 0 },
	{ "falloffPlacement", 25, 0, 0, 0 },
	{ "attenuation", 5, 0, 0, 0 },
	{ "fallOffA", 6, 0, 0, 0 },
	{ "fallOffB", 7, 0, 0, 0 },
	{ "spotMatrix0", 8, 0, 0, 0 },
	{ "spotMatrix1", 9, 0, 0, 0 },
	{ "spotMatrix2", 10, 0, 0, 0 },
	{ "spotMatrix3", 11, 0, 0, 0 },
	{ "spotAABB", 12, 0, 0, 0 },
	{ "coneControl1", 13, 0, 0, 0 },
	{ "coneControl2", 14, 0, 0, 0 },
	{ "spotCookieSlideControl", 15, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeConstantSource s_codeConsts[] =
{
	{ "nearPlane", 230, s_nearPlaneConsts, 0, 0 },
	{ "sun", 230, s_sunConsts, 0, 0 },
	{ "light", 230, s_lightConsts, 0, 0 },

	{ "baseLightingCoords", 78, 0, 0, 0 },
	{ "lightingLookupScale", 53, 0, 0, 0 },
	{ "debugBumpmap", 54, 0, 0, 0 },
	{ "pixelCostFracs", 34, 0, 0, 0 },
	{ "pixelCostDecode", 35, 0, 0, 0 },
	{ "materialColor", 55, 0, 0, 0 },
	{ "fogConsts", 56, 0, 0, 0 },
	{ "fogConsts2", 57, 0, 0, 0 },
	{ "fogColor", 58, 0, 0, 0 },
	{ "sunFogColor", 61, 0, 0, 0 },
	{ "sunFogDir", 60, 0, 0, 0 },
	{ "sunFog", 59, 0, 0, 0 },
	{ "glowSetup", 62, 0, 0, 0 },
	{ "glowApply", 63, 0, 0, 0 },
	{ "filterTap", 36, 0, 8, 1 },
	{ "codeMeshArg", 76, 0, 2, 1 },
	{ "renderTargetSize", 21, 0, 0, 0 },
	{ "vposx_to_world", 22, 0, 0, 0 },
	{ "vposy_to_world", 23, 0, 0, 0 },
	{ "vpos1_to_world", 24, 0, 0, 0 },
	{ "shadowmapSwitchPartition", 47, 0, 0, 0 },
	{ "shadowmapScale", 48, 0, 0, 0 },
	{ "shadowmapPolygonOffset", 20, 0, 0, 0 },
	{ "shadowParms", 19, 0, 0, 0 },
	{ "zNear", 49, 0, 0, 0 },
	{ "clipSpaceLookupScale", 72, 0, 0, 0 },
	{ "clipSpaceLookupOffset", 73, 0, 0, 0 },
	{ "dofEquationViewModelAndFarBlur", 26, 0, 0, 0 },
	{ "dofEquationScene", 27, 0, 0, 0 },
	{ "dofLerpScale", 28, 0, 0, 0 },
	{ "dofLerpBias", 29, 0, 0, 0 },
	{ "dofRowDelta", 30, 0, 0, 0 },
	{ "depthFromClip", 75, 0, 0, 0 },
	{ "outdoorFeatherParms", 67, 0, 0, 0 },
	{ "skyTransition", 68, 0, 0, 0 },
	{ "envMapParms", 69, 0, 0, 0 },
	{ "waterParms", 80, 0, 0, 0 },
	{ "colorMatrixR", 44, 0, 0, 0 },
	{ "colorMatrixG", 45, 0, 0, 0 },
	{ "colorMatrixB", 46, 0, 0, 0 },
	{ "colorBias", 64, 0, 0, 0 },
	{ "colorTintBase", 65, 0, 0, 0 },
	{ "colorTintDelta", 66, 0, 0, 0 },
	{ "gameTime", 32, 0, 0, 0 },
	{ "alphaFade", 33, 0, 0, 0 },
	{ "destructibleParms", 114, 0, 0, 0 },
	{ "particleCloudColor", 31, 0, 0, 0 },
	{ "particleCloudMatrix", 74, 0, 0, 0 },
	{ "worldMatrix", 197, 0, 0, 0 },
	{ "inverseWorldMatrix", 198, 0, 0, 0 },
	{ "transposeWorldMatrix", 199, 0, 0, 0 },
	{ "inverseTransposeWorldMatrix", 200, 0, 0, 0 },
	{ "viewMatrix", 201, 0, 0, 0 },
	{ "inverseViewMatrix", 202, 0, 0, 0 },
	{ "transposeViewMatrix", 203, 0, 0, 0 },
	{ "inverseTransposeViewMatrix", 204, 0, 0, 0 },
	{ "projectionMatrix", 205, 0, 0, 0 },
	{ "inverseProjectionMatrix", 206, 0, 0, 0 },
	{ "transposeProjectionMatrix", 207, 0, 0, 0 },
	{ "inverseTransposeProjectionMatrix", 208, 0, 0, 0 },
	{ "worldViewMatrix", 209, 0, 0, 0 },
	{ "inverseWorldViewMatrix", 210, 0, 0, 0 },
	{ "transposeWorldViewMatrix", 211, 0, 0, 0 },
	{ "inverseTransposeWorldViewMatrix", 212, 0, 0, 0 },
	{ "viewProjectionMatrix", 213, 0, 0, 0 },
	{ "inverseViewProjectionMatrix", 214, 0, 0, 0 },
	{ "transposeViewProjectionMatrix", 215, 0, 0, 0 },
	{ "inverseTransposeViewProjectionMatrix", 216, 0, 0, 0 },
	{ "worldViewProjectionMatrix", 217, 0, 0, 0 },
	{ "inverseWorldViewProjectionMatrix", 218, 0, 0, 0 },
	{ "transposeWorldViewProjectionMatrix", 219, 0, 0, 0 },
	{ "inverseTransposeWorldViewProjectionMatrix", 220, 0, 0, 0 },
	{ "shadowLookupMatrix", 221, 0, 0, 0 },
	{ "inverseShadowLookupMatrix", 222, 0, 0, 0 },
	{ "transposeShadowLookupMatrix", 223, 0, 0, 0 },
	{ "inverseTransposeShadowLookupMatrix", 224, 0, 0, 0 },
	{ "worldOutdoorLookupMatrix", 225, 0, 0, 0 },
	{ "inverseWorldOutdoorLookupMatrix", 226, 0, 0, 0 },
	{ "transposeWorldOutdoorLookupMatrix", 227, 0, 0, 0 },
	{ "inverseTransposeWorldOutdoorLookupMatrix", 228, 0, 0, 0 },
	{ "windDirection", 79, 0, 0, 0 },
	{ "variantWindSpring", 98, 0, 16, 1 },
	{ "u_customWindCenter", 192, 0, 0, 0 },
	{ "u_customWindSpring", 193, 0, 0, 0 },
	{ "grassParms", 81, 0, 0, 0 },
	{ "grassForce0", 82, 0, 0, 0 },
	{ "grassForce1", 83, 0, 0, 0 },
	{ "grassWindForce0", 84, 0, 0, 0 },
	{ "cloudWorldArea", 115, 0, 0, 0 },
	{ "waterScroll", 116, 0, 0, 0 },
	{ "motionblurDirectionAndMagnitude", 85, 0, 0, 0 },
	{ "flameDistortion", 86, 0, 0, 0 },
	{ "bloomScale", 87, 0, 0, 0 },
	{ "overlayTexCoord", 88, 0, 0, 0 },
	{ "colorBias1", 89, 0, 0, 0 },
	{ "colorTintBase1", 90, 0, 0, 0 },
	{ "colorTintDelta1", 91, 0, 0, 0 },
	{ "fadeEffect", 92, 0, 0, 0 },
	{ "viewportDimensions", 93, 0, 0, 0 },
	{ "framebufferRead", 94, 0, 0, 0 },
	{ "resizeParams1", 95, 0, 0, 0 },
	{ "resizeParams2", 96, 0, 0, 0 },
	{ "resizeParams3", 97, 0, 0, 0 },
	{ "crossFadeAlpha", 117, 0, 0, 0 },
	{ "__characterCharredAmount", 118, 0, 0, 0 },
	{ "treeCanopyParms", 119, 0, 0, 0 },
	{ "marksHitNormal", 120, 0, 0, 0 },
	{ "postFxControl0", 121, 0, 0, 0 },
	{ "postFxControl1", 122, 0, 0, 0 },
	{ "postFxControl2", 123, 0, 0, 0 },
	{ "postFxControl3", 124, 0, 0, 0 },
	{ "postFxControl4", 125, 0, 0, 0 },
	{ "postFxControl5", 126, 0, 0, 0 },
	{ "postFxControl6", 127, 0, 0, 0 },
	{ "postFxControl7", 128, 0, 0, 0 },
	{ "postFxControl8", 129, 0, 0, 0 },
	{ "postFxControl9", 130, 0, 0, 0 },
	{ "postFxControlA", 131, 0, 0, 0 },
	{ "postFxControlB", 132, 0, 0, 0 },
	{ "postFxControlC", 133, 0, 0, 0 },
	{ "postFxControlD", 134, 0, 0, 0 },
	{ "postFxControlE", 135, 0, 0, 0 },
	{ "postFxControlF", 136, 0, 0, 0 },
	{ "hdrControl0", 137, 0, 0, 0 },
	{ "hdrControl1", 138, 0, 0, 0 },
	{ "glightPosXs", 139, 0, 0, 0 },
	{ "glightPosYs", 140, 0, 0, 0 },
	{ "glightPosZs", 141, 0, 0, 0 },
	{ "glightFallOffs", 142, 0, 0, 0 },
	{ "glightReds", 143, 0, 0, 0 },
	{ "glightGreens", 144, 0, 0, 0 },
	{ "glightBlues", 145, 0, 0, 0 },
	{ "dlightPosition", 146, 0, 0, 0 },
	{ "dlightDiffuse", 147, 0, 0, 0 },
	{ "dlightSpecular", 148, 0, 0, 0 },
	{ "dlightAttenuation", 149, 0, 0, 0 },
	{ "dlightFallOff", 150, 0, 0, 0 },
	{ "dlightSpotMatrix0", 151, 0, 0, 0 },
	{ "dlightSpotMatrix1", 152, 0, 0, 0 },
	{ "dlightSpotMatrix2", 153, 0, 0, 0 },
	{ "dlightSpotMatrix3", 154, 0, 0, 0 },
	{ "dlightSpotDir", 155, 0, 0, 0 },
	{ "dlightSpotFactors", 156, 0, 0, 0 },
	{ "dlightShadowLookupMatrix0", 157, 0, 0, 0 },
	{ "dlightShadowLookupMatrix1", 158, 0, 0, 0 },
	{ "dlightShadowLookupMatrix2", 159, 0, 0, 0 },
	{ "dlightShadowLookupMatrix3", 160, 0, 0, 0 },
	{ "cloudLayerControl0", 161, 0, 0, 0 },
	{ "cloudLayerControl1", 162, 0, 0, 0 },
	{ "cloudLayerControl2", 163, 0, 0, 0 },
	{ "cloudLayerControl3", 164, 0, 0, 0 },
	{ "cloudLayerControl4", 165, 0, 0, 0 },
	{ "heroLightingR", 166, 0, 0, 0 },
	{ "heroLightingG", 167, 0, 0, 0 },
	{ "heroLightingB", 168, 0, 0, 0 },
	{ "lightHeroScale", 169, 0, 0, 0 },
	{ "cinematicBlurBox", 170, 0, 0, 0 },
	{ "cinematicBlurBox2", 171, 0, 0, 0 },
	{ "adsZScale", 172, 0, 0, 0 },
	{ "ui3dUVSetup0", 173, 0, 0, 0 },
	{ "ui3dUVSetup1", 174, 0, 0, 0 },
	{ "ui3dUVSetup2", 175, 0, 0, 0 },
	{ "ui3dUVSetup3", 176, 0, 0, 0 },
	{ "ui3dUVSetup4", 177, 0, 0, 0 },
	{ "ui3dUVSetup5", 178, 0, 0, 0 },
	{ "__characterDissolveColor", 179, 0, 0, 0 },
	{ "cameraLook", 180, 0, 0, 0 },
	{ "cameraUp", 181, 0, 0, 0 },
	{ "cameraSide", 182, 0, 0, 0 },
	{ "scriptVector0", 183, 0, 0, 0 },
	{ "scriptVector1", 184, 0, 0, 0 },
	{ "scriptVector2", 185, 0, 0, 0 },
	{ "scriptVector3", 186, 0, 0, 0 },
	{ "scriptVector4", 187, 0, 0, 0 },
	{ "scriptVector5", 188, 0, 0, 0 },
	{ "scriptVector6", 189, 0, 0, 0 },
	{ "scriptVector7", 190, 0, 0, 0 },
	{ "eyeOffset", 191, 0, 0, 0 },
	{ "skyColorMultiplier", 194, 0, 0, 0 },
	{ "extraCamParam", 195, 0, 0, 0 },
	{ "emblemLUTSelector", 196, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

CodeConstantSource s_defaultCodeConsts[] =
{
	{ "nearPlaneOrg", 16, 0, 0, 0 },
	{ "nearPlaneDx", 17, 0, 0, 0 },
	{ "nearPlaneDy", 18, 0, 0, 0 },
	{ "sunPosition", 50, 0, 0, 0 },
	{ "sunDiffuse", 51, 0, 0, 0 },
	{ "sunSpecular", 52, 0, 0, 0 },
	{ "lightPosition", 0, 0, 0, 0 },
	{ "lightDiffuse", 1, 0, 0, 0 },
	{ "lightSpecular", 2, 0, 0, 0 },
	{ "lightSpotDir", 3, 0, 0, 0 },
	{ "lightSpotFactors", 4, 0, 0, 0 },
	{ "lightFalloffPlacement", 25, 0, 0, 0 },
	{ "lightAttenuation", 5, 0, 0, 0 },
	{ "lightFallOffA", 6, 0, 0, 0 },
	{ "lightFallOffB", 7, 0, 0, 0 },
	{ "lightSpotMatrix0", 8, 0, 0, 0 },
	{ "lightSpotMatrix1", 9, 0, 0, 0 },
	{ "lightSpotMatrix2", 10, 0, 0, 0 },
	{ "lightSpotMatrix3", 11, 0, 0, 0 },
	{ "lightSpotAABB", 12, 0, 0, 0 },
	{ "lightConeControl1", 13, 0, 0, 0 },
	{ "lightConeControl2", 14, 0, 0, 0 },
	{ "lightSpotCookieSlideControl", 15, 0, 0, 0 },
	{ "spotShadowmapPixelAdjust", 70, 0, 0, 0 },
	{ "dlightSpotShadowmapPixelAdjust", 71, 0, 0, 0 },

	{ nullptr, 0, 0, 0, 0 },
};

size_t g_MaterialFileSize = 0;
void* rtn_MaterialLoad = (void*)0x00532AAE;

//
// Used to store the current materials rawfile size globally for use by Material_LoadRaw
//
void __declspec(naked) mfh_MaterialLoad()
{
	_asm
	{
		mov g_MaterialFileSize, edi
		push eax
		push edi
		push esi
		call FS_Read

		jmp rtn_MaterialLoad
	}
}

void* MapOffsetToPointer(void* origin, int offset)
{
	return (void*)((char*)origin + offset);
}

DWORD MapPointerToOffset(void* origin, void* offset)
{
	return (DWORD)((char*)offset - (char*)origin);
}

std::vector<techsetOverride> techsetOverrideList;

Material_LoadRaw_t* o_Material_LoadRaw = (Material_LoadRaw_t *)0x005325F0;
int Material_LoadRaw(MaterialRaw *mtlRaw, unsigned int materialType, int imageTrack)
{
	if (!mtlRaw->constantCount)
		return o_Material_LoadRaw(mtlRaw, materialType, imageTrack);

	const char* techsetOverride = nullptr;
	for (unsigned int i = 0; i < techsetOverrideList.size(); i++)
	{
		if (strcmp((char*)MapOffsetToPointer(mtlRaw, mtlRaw->techSetNameOffset), techsetOverrideList[i].key.c_str()) == 0)
		{
			printf("overriding technique %s\a\n", techsetOverrideList[i].replacement.c_str());
			techsetOverride = techsetOverrideList[i].replacement.c_str();
			break;
		}
	}

	if (!techsetOverride)
	{
		return o_Material_LoadRaw(mtlRaw, materialType, imageTrack);
	}

	MaterialConstantDefRaw* constantTable = (MaterialConstantDefRaw*)MapOffsetToPointer(mtlRaw, mtlRaw->constantTableOffset);

	bool insertConstant = true;
	for (int i = 0; i < mtlRaw->constantCount; i++)
	{
		if (strcmp("envMapParms", (char*)MapOffsetToPointer(mtlRaw, constantTable[i].nameOffset)) == 0)
		{
			insertConstant = false;
			break;
		}
	}

	int newsize = g_MaterialFileSize + 32768;
	MaterialRaw* mtlNew = (MaterialRaw*)Hunk_AllocTempMemory(newsize);
	memcpy(mtlNew, mtlRaw, sizeof(MaterialRaw));
	BYTE* dest = (BYTE*)&mtlNew[1];

	MaterialTextureDefRaw* oldTextureTable = (MaterialTextureDefRaw*)MapOffsetToPointer(mtlRaw, mtlRaw->textureTableOffset);
	MaterialTextureDefRaw* textureTable = (MaterialTextureDefRaw*)dest;
	mtlNew->textureTableOffset = MapPointerToOffset(mtlNew, textureTable);
	memcpy(dest, MapOffsetToPointer(mtlRaw, mtlRaw->textureTableOffset), sizeof(MaterialTextureDefRaw) * mtlRaw->textureCount);
	dest += sizeof(MaterialTextureDefRaw) * mtlRaw->textureCount;

	MaterialConstantDefRaw* oldConstantTable = (MaterialConstantDefRaw*)MapOffsetToPointer(mtlRaw, mtlRaw->constantTableOffset);
	constantTable = (MaterialConstantDefRaw*)dest;
	mtlNew->constantTableOffset = MapPointerToOffset(mtlNew, constantTable);
	memcpy(dest, MapOffsetToPointer(mtlRaw, mtlRaw->constantTableOffset), sizeof(MaterialConstantDefRaw) * mtlRaw->constantCount);
	dest += sizeof(MaterialConstantDefRaw) * mtlRaw->constantCount;

	if (insertConstant)
	{
		MaterialConstantDefRaw def;
		def.nameOffset = NULL;
		def.literal[0] = 1.0f;
		def.literal[1] = 1.0f;
		def.literal[2] = 1.0f;
		def.literal[3] = 1.0f;
		memcpy(dest, &def, sizeof(MaterialConstantDefRaw));
		dest += sizeof(MaterialConstantDefRaw);
	}

	char* techSetName = (char*)dest;
	mtlNew->techSetNameOffset = MapPointerToOffset(mtlNew, techSetName);
	strcpy((char*)dest, techsetOverride);
	dest += strlen(techSetName) + 1;

	char* name = (char*)dest;
	mtlNew->info.nameOffset = MapPointerToOffset(mtlNew, name);
	strcpy((char*)dest, (char*)MapOffsetToPointer(mtlRaw, mtlRaw->info.nameOffset));
	dest += strlen(name) + 1;

	char* refImageName = (char*)dest;
	mtlNew->info.refImageNameOffset = MapPointerToOffset(mtlNew, refImageName);
	strcpy((char*)dest, (char*)MapOffsetToPointer(mtlRaw, mtlRaw->info.refImageNameOffset));
	dest += strlen(refImageName) + 1;

	for (int i = 0; i < mtlRaw->textureCount; i++)
	{
		char* textureName = (char*)dest; //contains strings like "colorMap", "normalMap" etc.
		textureTable[i].nameOffset = MapPointerToOffset(mtlNew, textureName);
		strcpy((char*)dest, (char*)MapOffsetToPointer(mtlRaw, oldTextureTable[i].nameOffset));
		dest += strlen(textureName) + 1;

		char* imageName = (char*)dest;
		textureTable[i].imageNameOffset = MapPointerToOffset(mtlNew, imageName);
		strcpy((char*)dest, (char*)MapOffsetToPointer(mtlRaw, oldTextureTable[i].imageNameOffset));
		dest += strlen(imageName) + 1;
	}

	for (int i = 0; i < mtlRaw->constantCount; i++)
	{
		name = (char*)dest;
		constantTable[i].nameOffset = MapPointerToOffset(mtlNew, name);
		strcpy((char*)dest, (char*)MapOffsetToPointer(mtlRaw, oldConstantTable[i].nameOffset));
		dest += strlen(name) + 1;
	}

	if (insertConstant)
	{
		name = (char*)dest;
		constantTable[mtlNew->constantCount].nameOffset = MapPointerToOffset(mtlNew, name);
		strcpy((char*)dest, "envMapParms");
		dest += strlen(name) + 1;
		mtlNew->constantCount = mtlRaw->constantCount + 1;
	}

	int result = o_Material_LoadRaw(mtlNew, materialType, imageTrack);
	Hunk_FreeTempMemory(mtlNew);

	return result;
}