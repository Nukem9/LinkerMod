#include "db_file_load.h"
#include "../shared/utility.h"

DB_LoadData g_load;

char *g_globalFFData;
size_t g_globalFFSize;

XAssetList g_varXAssetList;

XAssetList *varXAssetList;
ScriptStringList *varScriptStringList;

const char *varConstChar;
const char **varTempString;

int g_streamPosIndex;
StreamPosInfo g_streamPosStack[64];

int g_streamPosStackIndex;
XBlock *g_streamBlocks;
char *g_streamPos;
char *g_streamPosArray[7];
int g_streamDelayIndex;

XBlock g_blocks[7];

XAsset *varXAsset;
XAssetHeader *varXAssetHeader;

char mybuf[DBFILE_BUFFER_SIZE];
size_t readIn;

void TestIt(char *ffData, size_t size, HANDLE file)
{
	g_globalFFData = ffData;
	g_globalFFSize = size;
	readIn = 0;

	DB_LoadXFile((void *)file, "myff", g_blocks, nullptr, mybuf, 0, 0);
}

void WINAPI DB_FileReadCompletionDummyCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	printf("CB\n");
}

int DB_ReadData()
{
	ASSERT(g_load.compressBufferStart);
	ASSERT(g_load.f);

	if (g_load.interrupt)
		g_load.interrupt();

	void *fileBuffer = (void *)(g_load.compressBufferStart + g_load.overlapped.Offset % DBFILE_BUFFER_SIZE);

	DWORD bytesRead = 0;
	if (ReadFile(g_load.f, fileBuffer, 0x40000, &bytesRead, nullptr))
	{
		++g_load.outstandingReads;
		g_load.overlapped.Offset += 0x40000;
		return 1;
	}
	
	return 0;
	/*
	// Fake read directly from memory
	memcpy(fileBuffer, g_globalFFData + readIn, min(0x40000, g_globalFFSize - readIn));

	++g_load.outstandingReads;
	g_load.overlapped.Offset += 0x40000;
	return 1;
	*/
}

void DB_ReadXFileStage()
{
	if (g_load.f)
	{
		ASSERT(!g_load.outstandingReads);

		if (!DB_ReadData() && GetLastError() != ERROR_HANDLE_EOF)
			__debugbreak();
			//Com_Error(ERR_DROP, "Read error of file '%s'", g_load_filename);
	}
}

void DB_WaitXFileStage()
{
	ASSERT(g_load.f);
	ASSERT(g_load.outstandingReads > 0);

	--g_load.outstandingReads;

	//SleepEx(0xFFFFFFFF, 1);

	g_load.stream.avail_in += 0x40000;
	readIn = min(g_load.overlapped.Offset, g_globalFFSize);
}

void DB_LoadXFileSetSize(int size)
{
	ASSERT(g_load.deflateRemainingFileSize == 0);

	g_load.deflateRemainingFileSize = size;
}

bool DB_LoadXFile(void *f, const char *filename, XBlock *blocks, void(__cdecl *interrupt)(), char *buf, int allocType, int flags)
{
	ASSERT(!(reinterpret_cast<size_t>(buf) & 3));

	memset(&g_load, 0, sizeof(g_load));
	g_load.f			= f;
	g_load.filename		= filename;
	g_load.flags		= flags;
	g_load.blocks		= blocks;
	g_load.interrupt	= interrupt;
	g_load.allocType	= allocType;
	g_load.startTime	= GetTickCount();

	ASSERT(!g_load.compressBufferStart);
	ASSERT(g_load.f);
	ASSERT(buf);

	g_load.compressBufferStart	= buf;
	g_load.compressBufferEnd	= buf + DBFILE_BUFFER_SIZE;
	g_load.stream.next_in		= (Bytef *)buf;
	g_load.stream.avail_in		= 0;
	g_load.deflateBufferPos		= DEFLATE_BUFFER_SIZE;

	DB_LoadXFileInternal();
	return true;
}

void DB_LoadXFileData(char *pos, int size)
{
	ASSERT(size);
	ASSERT(g_load.f);
	ASSERT(!g_load.stream.avail_out);

readAgain:
	if (size <= 0)
		return;

	if (size + g_load.deflateBufferPos > DEFLATE_BUFFER_SIZE)
	{
		ASSERT(g_load.deflateBufferPos <= DEFLATE_BUFFER_SIZE);

		if (g_load.deflateBufferPos < DEFLATE_BUFFER_SIZE)
		{
			unsigned int bytesToCopy = DEFLATE_BUFFER_SIZE - g_load.deflateBufferPos;

			if (g_load.deflateBufferPos == (DEFLATE_BUFFER_SIZE - 1))
				*pos = g_load.deflateBuffer[g_load.deflateBufferPos];
			else
				memcpy(pos, &g_load.deflateBuffer[g_load.deflateBufferPos], bytesToCopy);

			g_load.deflateBufferPos += bytesToCopy;
			pos += bytesToCopy;
			size -= bytesToCopy;
		}

		int lastAvailOutSize = 0;
		int remainingSize = min(DEFLATE_BUFFER_SIZE, g_load.deflateRemainingFileSize);

		g_load.stream.avail_out = remainingSize;
		g_load.deflateBufferPos = DEFLATE_BUFFER_SIZE - remainingSize;
		g_load.stream.next_out = (Bytef *)&g_load.deflateBuffer[DEFLATE_BUFFER_SIZE - remainingSize];

		if (remainingSize < size)
			lastAvailOutSize = g_load.stream.avail_out;
		else
			lastAvailOutSize = size;

		size -= lastAvailOutSize;
		g_load.deflateRemainingFileSize -= g_load.stream.avail_out;

		while (true)
		{
			if (g_load.stream.avail_in)
			{
				int err = inflate(&g_load.stream, 2);

				if (err != Z_OK && err != Z_STREAM_END)
					__debugbreak();

				if (g_load.f)
				{
					ASSERT(static_cast<unsigned>(g_load.stream.next_in - (Bytef *)g_load.compressBufferStart) <= DBFILE_BUFFER_SIZE);

					if (g_load.stream.next_in == (Bytef *)g_load.compressBufferEnd)
						g_load.stream.next_in = (Bytef *)g_load.compressBufferStart;
				}

				if (!g_load.stream.avail_out)
				{
					ASSERT(lastAvailOutSize <= DEFLATE_BUFFER_SIZE);

					if (lastAvailOutSize == 1)
						*pos = g_load.deflateBuffer[g_load.deflateBufferPos];
					else
						memcpy(pos, &g_load.deflateBuffer[g_load.deflateBufferPos], lastAvailOutSize);

					g_load.deflateBufferPos += lastAvailOutSize;
					pos += lastAvailOutSize;
					goto readAgain;
				}

				if (err)
				{
					ASSERT(err == Z_OK);
					__debugbreak();
				}
			}

			DB_WaitXFileStage();
			DB_ReadXFileStage();
		}
	}

	if (size == 1)
		*pos = g_load.deflateBuffer[g_load.deflateBufferPos];
	else
		memcpy(pos, &g_load.deflateBuffer[g_load.deflateBufferPos], size);

	g_load.deflateBufferPos += size;
}

void DB_LoadXFileInternal()
{
	DB_ReadXFileStage();
	DB_WaitXFileStage();
	DB_ReadXFileStage();

	// Read fastfile header magic
	char magic[8];
	memcpy(magic, g_load.stream.next_in, sizeof(magic));

	g_load.stream.next_in	+= sizeof(magic);
	g_load.stream.avail_in	-= sizeof(magic);

	if (memcmp(magic, "IWff0100", 8) && memcmp(magic, "IWffu100", 8))
		__debugbreak();

	// Validate file version
	int version = *(int *)g_load.stream.next_in;

	g_load.stream.next_in	+= sizeof(int);
	g_load.stream.avail_in	-= sizeof(int);

	if (version != 473)
		__debugbreak();

	// Begin decompression
	int result = inflateInit(&g_load.stream);

	if (result == Z_STREAM_ERROR)
		__debugbreak();

	// Begin loading asset data
	XFile file;
	DB_LoadXFileSetSize(sizeof(XFile));
	DB_LoadXFileData((char *)&file, sizeof(XFile));
	DB_LoadXFileSetSize(file.size);

	DB_AllocXBlocks(file.blockSize, g_load.blocks);
	DB_InitStreams(g_load.blocks);

	Load_XAssetListCustom();
	DB_PushStreamPos(4);

	if (varXAssetList->assets)
	{
		varXAssetList->assets = AllocLoad_Align3();
		varXAsset = varXAssetList->assets;
		Load_XAssetArrayCustom(varXAssetList->assetCount);
	}

	DB_PopStreamPos();
	Load_DelayStream();

	// DB_LoadDelayedImages();
	ASSERT(g_load.compressBufferStart);
}

int Load_DelayStream()
{
	int result = 0;

	for (int i = 0; i < g_streamDelayIndex; ++i)
	{
		//DB_LoadXFileData(g_streamDelayArray[2 * i], dword_330A3C4[2 * i]);
		//result = i + 1;
	}

	return result;
}

void DB_AllocXBlocks(unsigned int *blockSize, XBlock *blocks)
{
	for (int blockIndex = 0; blockIndex < 7; blockIndex++)
	{
		unsigned int size = blockSize[blockIndex];

		if (size > 0)
		{
			char *buf = (char *)malloc(size);

			if (!buf)
				__debugbreak();

			memset(buf, 0, size);
			blocks[blockIndex].size = size;
			blocks[blockIndex].data = buf;
		}
	}
}

void DB_InitStreams(XBlock *blocks)
{
	g_streamBlocks = blocks;
	g_streamPos = blocks[0].data;
	g_streamPosIndex = 0;
	g_streamDelayIndex = 0;
	g_streamPosArray[0] = 0;
	g_streamPosStackIndex = 0;

	for (int i = 0; i < 7; ++i)
	{
		if (i)
			g_streamPosArray[i] = blocks[i].data;
	}
}

void Load_Stream(bool atStreamStart, const void *ptr, int size)
{
	ASSERT(atStreamStart == (ptr == DB_GetStreamPos()));

	if (atStreamStart && size)
	{
		if (g_streamPosIndex < 4)
		{
			if (g_streamPosIndex == 1)
			{
				memset((char *)ptr, 0, size);
			}
			else
			{
				ASSERT(g_streamDelayIndex >= 0 && g_streamDelayIndex < 0x1000);

				//__debugbreak();
				// (&g_streamDelayArray)[g_streamDelayIndex] = (struct StreamDelayInfo *)ptr;
				// dword_330A3C4[2 * g_streamDelayIndex++] = size;
			}
		}
		else
		{
			DB_LoadXFileData((char *)ptr, size);
		}

		DB_IncStreamPos(size);
	}
}

void Load_XStringCustom(const char **str)
{
	int numBytesLoaded = DB_LoadXFileDataNullTerminated((char *)*str);
	DB_IncStreamPos(numBytesLoaded);
}

void Load_TempStringCustom(const char **str)
{
	Load_XStringCustom(str);
	// *str = (const char *)v1;
}

void Load_TempString(bool atStreamStart)
{
	Load_Stream(atStreamStart, varTempString, 4);

	if (*(DWORD *)varTempString)
	{
		if (*(DWORD *)varTempString == -1)
		{
			*(char **)varTempString = AllocLoad_raw_byte();
			varConstChar = *(char **)varTempString;
			Load_TempStringCustom(varTempString);
		}
		else
		{
			DB_ConvertOffsetToPointer(varTempString);
		}
	}
}

void Load_TempStringArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varTempString, 4 * count);

	const char **var = varTempString;
	for (int i = 0; i < count; ++i)
	{
		varTempString = var;
		Load_TempString(false);
		var++;
	}
}

void Load_ScriptStringList(bool atStreamStart)
{
	Load_Stream(atStreamStart, varScriptStringList, sizeof(ScriptStringList));
	DB_PushStreamPos(4);

	if (varScriptStringList->strings)
	{
		varScriptStringList->strings = (const char **)AllocLoad_Align3();
		varTempString = varScriptStringList->strings;
		Load_TempStringArray(true, varScriptStringList->count);
	}

	DB_PopStreamPos();
}

void Load_XAssetListCustom()
{
	varXAssetList = &g_varXAssetList;
	DB_LoadXFileData((char *)&g_varXAssetList, sizeof(XAssetList));
	DB_PushStreamPos(4);
	varScriptStringList = &varXAssetList->stringList;
	Load_ScriptStringList(false);
	DB_PopStreamPos();
}

void DB_SetStreamIndex(unsigned int index)
{
	if (index != g_streamPosIndex)
	{
		g_streamPosArray[g_streamPosIndex] = g_streamPos;
		g_streamPosIndex = index;
		g_streamPos = g_streamPosArray[index];
		g_streamPosArray[index] = 0;
	}
}

void DB_IncStreamPos(int size)
{
	ASSERT(g_streamPos);
	ASSERT(g_streamPos + size <= g_streamBlocks[g_streamPosIndex].data + g_streamBlocks[g_streamPosIndex].size);

	g_streamPos += size;
}

char *DB_AllocStreamPos(int alignment)
{
	ASSERT(g_streamPos);

	return g_streamPos = (char *)(~alignment & (unsigned int)&g_streamPos[alignment]);
}

char *DB_GetStreamPos()
{
	return g_streamPos;
}

void DB_PushStreamPos(unsigned int index)
{
	ASSERT(index < ARRAYSIZE(g_streamPosArray));
	ASSERT(g_streamPosIndex < ARRAYSIZE(g_streamPosArray));
	ASSERT(g_streamPosStackIndex < ARRAYSIZE(g_streamPosStack));

	g_streamPosStack[g_streamPosStackIndex++].index = g_streamPosIndex;
	DB_SetStreamIndex(index);
	g_streamPosStack[g_streamPosStackIndex].pos = g_streamPos;
}

void DB_PopStreamPos()
{
	ASSERT(g_streamPosStackIndex > 0);

	--g_streamPosStackIndex;

	if (!g_streamPosIndex)
		g_streamPos = g_streamPosStack[g_streamPosStackIndex].pos;

	DB_SetStreamIndex(g_streamPosStack[g_streamPosStackIndex].index);
}

const void **DB_InsertPointer()
{
	DB_PushStreamPos(4);
	char *pData = DB_AllocStreamPos(3);
	DB_IncStreamPos(4);
	DB_PopStreamPos();

	return (const void **)pData;
}

void DB_ConvertOffsetToPointer(void *data)
{
	*(DWORD *)data = (DWORD)&g_streamBlocks[(unsigned int)(*(DWORD *)data - 1) >> 29].data[(*(DWORD *)data - 1) & 0x1FFFFFFF];
}

void DB_ConvertOffsetToAlias(void *data)
{
	int offset = *(int *)data;

	ASSERT((offset && (offset != 0xFFFFFFFF) && (offset != 0xFFFFFFFE)));

	*(DWORD *)data = *(DWORD *)&g_streamBlocks[(unsigned int)(offset - 1) >> 29].data[(offset - 1) & 0x1FFFFFFF];
}

int DB_LoadXFileDataNullTerminated(char *pos)
{
	char value;
	char *origPos = pos;

	do
	{
		if (g_load.deflateBufferPos == 32768)
			DB_LoadXFileData(pos, sizeof(char));
		else
			*pos = g_load.deflateBuffer[g_load.deflateBufferPos++];

		value = *pos++;
	} while (value);

	return pos - origPos;
}

XAsset *AllocLoad_Align3()
{
	return (XAsset *)DB_AllocStreamPos(3);
}

char *AllocLoad_raw_byte()
{
	return DB_AllocStreamPos(0);
}

void Load_XAssetArrayCustom(int count)
{
	Load_Stream(true, varXAsset, sizeof(XAsset) * count);

	XAsset *var = varXAsset;
	for (int i = 0; true; i++)
	{
		if (i < count)
		{
			varXAsset = var;
			Load_XAsset(false);
			var++;

			if (!g_load.abort)
				continue;
		}
		break;
	}
}

void Load_XAsset(bool atStreamStart)
{
	Load_Stream(atStreamStart, varXAsset, sizeof(XAsset));
	varXAssetHeader = &varXAsset->header;
	Load_XAssetHeader(0);
}

void *varRandomPtr;
void *varRandom;

void Load_RandomPointer(bool atStreamStart)
{
	Load_Stream(atStreamStart, varRandomPtr, 4);
	DB_PushStreamPos(0);

	// ...

	DB_PopStreamPos();
}

struct RawFile
{
	const char *name;
	int len;
	const char *buffer;
};

void *varRawFilePtr;
RawFile *varRawFile;
void *varXString;

void *varDWORD;
void *varuint;
void *varfloat;
void *varbyte;

void Load_DWORDArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varDWORD, count * sizeof(DWORD));
}

void Load_uint(bool atStreamStart)
{
	Load_Stream(atStreamStart, varuint, sizeof(unsigned int));
}

void Load_floatArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varfloat, count * sizeof(float));
}

void Load_byteArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varbyte, count * sizeof(unsigned char));
}

#define DECLARE_LOAD_FUNCTION_PTR(Name, Ptr)										\
void Load_##Ptr(bool atStreamStart)													\
{																					\
	Load_Stream(atStreamStart, var##Ptr, sizeof(var##Ptr));							\
	DB_PushStreamPos(0);															\
																					\
	DWORD value = *(DWORD *)var##Ptr;												\
	if (value)																		\
	{																				\
		if (value != -1 && value != -2)												\
		{																			\
			DB_ConvertOffsetToAlias(var##Ptr);										\
		}																			\
		else																		\
		{																			\
			*(DWORD *)var##Ptr = (DWORD)AllocLoad_Align3();							\
			var##Name = *(void **)var##Ptr;											\
																					\
			const void **v1 = (value == -2) ? DB_InsertPointer() : nullptr;			\
																					\
			Load_##Name(true);														\
			/* Load_##Name Asset((GfxImage **)var##Name Ptr); */					\
																					\
			if (v1)																	\
				*v1 = *(const void **)var##Ptr;										\
		}																			\
	}																				\
																					\
	DB_PopStreamPos();																\
}

//////////////////////////////////////
// ----- ASSET_TYPE_MATERIAL ----- //
//////////////////////////////////////
void *varMaterialTechniqueSetPtr;

void *varMaterialHandle;
void *varMaterial;
void *varMaterialInfo;

DECLARE_LOAD_FUNCTION_PTR(Material, MaterialHandle)

void Load_Material(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterial, 192);
	DB_PushStreamPos(4);

	varMaterialInfo = varMaterial;
	Load_MaterialInfo(false);

	varMaterialTechniqueSetPtr = (char *)varMaterial + 176;
	Load_MaterialTechniqueSetPtr(false);
	
	if (*((DWORD *)varMaterial + 45))
	{
		if (*((DWORD *)varMaterial + 45) == -1)
		{
			*((DWORD *)varMaterial + 45) = (DWORD)AllocLoad_Align3();
			//varMaterialTextureDef = (void *)*((DWORD *)varMaterial + 45);
			//Load_MaterialTextureDefArray(1, *((BYTE *)varMaterial + 170));
		}
		else
		{
			DB_ConvertOffsetToPointer((char *)varMaterial + 180);
		}
	}
	if (*((DWORD *)varMaterial + 46))
	{
		if (*((DWORD *)varMaterial + 46) == -1)
		{
			//*((DWORD *)varMaterial + 46) = AllocLoad_GfxPackedVertex0();
			//varMaterialConstantDef = (void *)*((DWORD *)varMaterial + 46);
			//Load_MaterialConstantDefArray(1, *((BYTE *)varMaterial + 171));
		}
		else
		{
			DB_ConvertOffsetToPointer((char *)varMaterial + 184);
		}
	}
	if (*((DWORD *)varMaterial + 47))
	{
		if (*((DWORD *)varMaterial + 47) == -1)
		{
			*((DWORD *)varMaterial + 47) = (DWORD)AllocLoad_Align3();
			//varGfxStateBits = (void *)*((DWORD *)varMaterial + 47);
			//Load_GfxStateBitsArray(1, *((BYTE *)varMaterial + 172));
		}
		else
		{
			DB_ConvertOffsetToPointer((char *)varMaterial + 188);
		}
	}

	DB_PopStreamPos();
}

void Load_MaterialInfo(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialInfo, 40);

	varXString = varMaterialInfo;
	Load_XString(false);
}

//////////////////////////////////////
// --- ASSET_TYPE_TECHNIQUE_SET --- //
//////////////////////////////////////
void *varMaterialTechniqueSet;

void *varMaterialTechniquePtr;
void *varMaterialTechnique;

void *varMaterialPass;

void *varMaterialVertexDeclaration;
void *varMaterialShaderArgument;

void *varMaterialVertexShaderPtr;
void *varMaterialVertexShader;
void *varMaterialVertexShaderProgram;
void *varGfxVertexShaderLoadDef;

void *varMaterialPixelShaderPtr;
void *varMaterialPixelShader;
void *varMaterialPixelShaderProgram;
void *varGfxPixelShaderLoadDef;

void *varMaterialArgumentDef;
void *varMaterialArgumentCodeConst;

DECLARE_LOAD_FUNCTION_PTR(MaterialTechniqueSet, MaterialTechniqueSetPtr)

void Load_MaterialTechniqueSet(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialTechniqueSet, 528);
	DB_PushStreamPos(4);

	varXString = varMaterialTechniqueSet;
	Load_XString(false);

	varMaterialTechniquePtr = (char *)varMaterialTechniqueSet + 8;
	Load_MaterialTechniquePtrArray(false, 130);

	DB_PopStreamPos();
}

void Load_MaterialTechniquePtrArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varMaterialTechniquePtr, count * sizeof(varMaterialTechniquePtr));

	void *var = varMaterialTechniquePtr;
	for (int i = 0; i < count; i++)
	{
		varMaterialTechniquePtr = var;
		Load_MaterialTechniquePtr(false);
		var = (char *)var + 4;
	}
}

void Load_MaterialTechniquePtr(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialTechniquePtr, sizeof(varMaterialTechniquePtr));

	if (*(DWORD *)varMaterialTechniquePtr)
	{
		if (*(DWORD *)varMaterialTechniquePtr == -1)
		{
			*(DWORD *)varMaterialTechniquePtr = (DWORD)AllocLoad_Align3();
			varMaterialTechnique = *(void **)varMaterialTechniquePtr;
			Load_MaterialTechnique(true);
		}
		else
		{
			DB_ConvertOffsetToPointer(varMaterialTechniquePtr);
		}
	}
}

void Load_MaterialTechnique(bool atStreamStart)
{
	ASSERT(atStreamStart);

	Load_Stream(true, varMaterialTechnique, 8);
	ASSERT(DB_GetStreamPos() == (char *)varMaterialTechnique + 8);
	// ASSERT(DB_GetStreamPos() == reinterpret_cast<byte *>(varMaterialTechnique->passArray));

	varMaterialPass = (char *)varMaterialTechnique + 8;
	Load_MaterialPassArray(true, *(WORD *)((char *)varMaterialTechnique + 6));

	varXString = varMaterialTechnique;
	Load_XString(false);
}

void Load_MaterialPassArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varMaterialPass, count * 20);

	void *var = varMaterialPass;
	for (int i = 0; i < count; ++i)
	{
		varMaterialPass = var;
		Load_MaterialPass(false);
		var = (char *)var + 20;
	}
}

void Load_MaterialPass(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialPass, 20);

	if (*(DWORD *)varMaterialPass)
	{
		if (*(DWORD *)varMaterialPass == -1)
		{
			*(DWORD *)varMaterialPass = (DWORD)AllocLoad_Align3();
			varMaterialVertexDeclaration = *(void **)varMaterialPass;
			Load_MaterialVertexDeclaration(true);
			// Load_BuildVertexDecl((MaterialVertexDeclaration **)varMaterialPass);
		}
		else
		{
			DB_ConvertOffsetToPointer(varMaterialPass);
		}
	}

	varMaterialVertexShaderPtr = (char *)varMaterialPass + 4;
	Load_MaterialVertexShaderPtr(false);
	varMaterialPixelShaderPtr = (char *)varMaterialPass + 8;
	Load_MaterialPixelShaderPtr(false);

	if (*((DWORD *)varMaterialPass + 4))
	{
		*((DWORD *)varMaterialPass + 4) = (DWORD)AllocLoad_Align3();
		varMaterialShaderArgument = (void *)*((DWORD *)varMaterialPass + 4);

		Load_MaterialShaderArgumentArray(
			true,
			*((BYTE *)varMaterialPass + 14) + *((BYTE *)varMaterialPass + 13) + *((BYTE *)varMaterialPass + 12));
	}
}

void Load_MaterialVertexDeclaration(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialVertexDeclaration, 108);
}

void Load_MaterialVertexShaderPtr(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialVertexShaderPtr, sizeof(varMaterialVertexShaderPtr));

	if (*(DWORD *)varMaterialVertexShaderPtr)
	{
		if (*(DWORD *)varMaterialVertexShaderPtr == -1)
		{
			*(DWORD *)varMaterialVertexShaderPtr = (DWORD)AllocLoad_Align3();
			varMaterialVertexShader = *(void **)varMaterialVertexShaderPtr;
			Load_MaterialVertexShader(true);
		}
		else
		{
			DB_ConvertOffsetToPointer(varMaterialVertexShaderPtr);
		}
	}
}

void Load_MaterialVertexShader(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialVertexShader, 16);

	varXString = varMaterialVertexShader;
	Load_XString(false);

	varMaterialVertexShaderProgram = (char *)varMaterialVertexShader + 4;
	Load_MaterialVertexShaderProgram(false);
}

void Load_MaterialVertexShaderProgram(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialVertexShaderProgram, 12);

	varGfxVertexShaderLoadDef = (char *)varMaterialVertexShaderProgram + 4;
	Load_GfxVertexShaderLoadDef(false);

	// Load_CreateMaterialVertexShader(
	// 	(GfxVertexShaderLoadDef *)((char *)varMaterialVertexShaderProgram + 4),
	// 	(MaterialVertexShader *)varMaterialVertexShader);
}

void Load_GfxVertexShaderLoadDef(bool atStreamStart)
{
	Load_Stream(atStreamStart, varGfxVertexShaderLoadDef, 8);

	if (*(DWORD *)varGfxVertexShaderLoadDef)
	{
		*(DWORD *)varGfxVertexShaderLoadDef = (DWORD)AllocLoad_Align3();
		varDWORD = *(void **)varGfxVertexShaderLoadDef;
		Load_DWORDArray(1, *((WORD *)varGfxVertexShaderLoadDef + 2));
	}
}

void Load_MaterialPixelShaderPtr(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialPixelShaderPtr, sizeof(varMaterialPixelShaderPtr));

	if (*(DWORD *)varMaterialPixelShaderPtr)
	{
		if (*(DWORD *)varMaterialPixelShaderPtr == -1)
		{
			*(DWORD *)varMaterialPixelShaderPtr = (DWORD)AllocLoad_Align3();
			varMaterialPixelShader = *(void **)varMaterialPixelShaderPtr;
			Load_MaterialPixelShader(true);
		}
		else
		{
			DB_ConvertOffsetToPointer(varMaterialPixelShaderPtr);
		}
	}
}

void Load_MaterialPixelShader(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialPixelShader, 16);

	varXString = varMaterialPixelShader;
	Load_XString(false);

	varMaterialPixelShaderProgram = (char *)varMaterialPixelShader + 4;
	Load_MaterialPixelShaderProgram(false);
}

void Load_MaterialPixelShaderProgram(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialPixelShaderProgram, 12);

	varGfxPixelShaderLoadDef = (char *)varMaterialPixelShaderProgram + 4;
	Load_GfxPixelShaderLoadDef(false);

	// Load_CreateMaterialPixelShader(
	// 	(GfxPixelShaderLoadDef *)((char *)varMaterialPixelShaderProgram + 4),
	// 	(MaterialPixelShader *)varMaterialPixelShader);
}

void Load_GfxPixelShaderLoadDef(bool atStreamStart)
{
	Load_Stream(atStreamStart, varGfxPixelShaderLoadDef, 8);

	if (*(DWORD *)varGfxPixelShaderLoadDef)
	{
		*(DWORD *)varGfxPixelShaderLoadDef = (DWORD)AllocLoad_Align3();
		varDWORD = *(void **)varGfxPixelShaderLoadDef;
		Load_DWORDArray(1, *((WORD *)varGfxPixelShaderLoadDef + 2));
	}
}

void Load_MaterialShaderArgumentArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varMaterialShaderArgument, 8 * count);

	void *var = varMaterialShaderArgument;
	for (int i = 0; i < count; ++i)
	{
		varMaterialShaderArgument = var;
		Load_MaterialShaderArgument(false);
		var = (char *)var + 8;
	}
}

void Load_MaterialShaderArgument(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialShaderArgument, 8);
	varMaterialArgumentDef = (char *)varMaterialShaderArgument + 4;
	Load_MaterialArgumentDef(false);
}

void Load_MaterialArgumentDef(bool atStreamStart)
{
	if (*(WORD *)varMaterialShaderArgument != 1 && *(WORD *)varMaterialShaderArgument != 7)
	{
		if (*(WORD *)varMaterialShaderArgument != 3 && *(WORD *)varMaterialShaderArgument != 5)
		{
			if (*(WORD *)varMaterialShaderArgument == 4)
			{
				if (atStreamStart)
				{
					varuint = varMaterialArgumentDef;
					Load_uint(atStreamStart);
				}
			}
			else if (atStreamStart)
			{
				varuint = varMaterialArgumentDef;
				Load_uint(atStreamStart);
			}
		}
		else if (atStreamStart)
		{
			varMaterialArgumentCodeConst = varMaterialArgumentDef;
			Load_MaterialArgumentCodeConst(atStreamStart);
		}
	}
	else if (*(DWORD *)varMaterialArgumentDef)
	{
		if (*(DWORD *)varMaterialArgumentDef == -1)
		{
			*(DWORD *)varMaterialArgumentDef = (DWORD)AllocLoad_Align3();
			varfloat = *(void **)varMaterialArgumentDef;
			Load_floatArray(true, 4);
		}
		else
		{
			DB_ConvertOffsetToPointer(varMaterialArgumentDef);
		}
	}
}

void Load_MaterialArgumentCodeConst(bool atStreamStart)
{
	Load_Stream(atStreamStart, varMaterialArgumentCodeConst, 4);
}

//////////////////////////////////////
// ------- ASSET_TYPE_IMAGE ------- //
//////////////////////////////////////
void *varGfxImagePtr;
void *varGfxImage;

void *varGfxTextureLoad;
void *varGfxImageLoadDef;

DECLARE_LOAD_FUNCTION_PTR(GfxImage, GfxImagePtr)

void Load_GfxImage(bool atStreamStart)
{
	Load_Stream(atStreamStart, varGfxImage, 52);
	DB_PushStreamPos(4);

	varXString = (char *)varGfxImage + 44;
	Load_XString(false);

	varGfxTextureLoad = varGfxImage;
	Load_GfxTextureLoad(false);

	DB_PopStreamPos();
}

void Load_GfxTextureLoad(bool atStreamStart)
{
	Load_Stream(atStreamStart, varGfxTextureLoad, 4);
	DB_PushStreamPos(0);

	if (*(DWORD *)varGfxTextureLoad)
	{
		DWORD value = *(DWORD *)varGfxTextureLoad;
		if (value != -1 && value != -2)
		{
			DB_ConvertOffsetToAlias(varGfxTextureLoad);
		}
		else
		{
			*(DWORD *)varGfxTextureLoad = (DWORD)AllocLoad_Align3();
			varGfxImageLoadDef = *(void **)varGfxTextureLoad;

			const void **v1 = (value == -2) ? DB_InsertPointer() : nullptr;

			Load_GfxImageLoadDef(true);
			// Load_Texture((GfxImageLoadDef **)varGfxTextureLoad, varGfxImage);

			if (v1)
				*v1 = *(const void **)varGfxTextureLoad;
		}
	}

	DB_PopStreamPos();
}

void Load_GfxImageLoadDef(bool atStreamStart)
{
	ASSERT(atStreamStart);

	Load_Stream(1, varGfxImageLoadDef, 12);
	ASSERT(DB_GetStreamPos() == (char *)varGfxImageLoadDef + 12);
	// ASSERT_TRUE(DB_GetStreamPos() == reinterpret_cast<byte *>(varGfxImageLoadDef->data));

	varbyte = (char *)varGfxImageLoadDef + 12;
	Load_byteArray(true, *((DWORD *)varGfxImageLoadDef + 2));
}

//////////////////////////////////////
// ------ ASSET_TYPE_RAWFILE ------ //
//////////////////////////////////////
void Load_RawFilePtr(bool atStreamStart)
{
	Load_Stream(atStreamStart, varRawFilePtr, 4);
	DB_PushStreamPos(0);

	if (*(DWORD *)varRawFilePtr)
	{
		DWORD value = *(DWORD *)varRawFilePtr;
		if (value != -1 && value != -2)
		{
			DB_ConvertOffsetToAlias(varRawFilePtr);
		}
		else
		{
			*(DWORD *)varRawFilePtr = (DWORD)AllocLoad_Align3();
			varRawFile = *(RawFile **)varRawFilePtr;

			const void **v1 = (value == -2) ? DB_InsertPointer() : nullptr;

			Load_RawFile(true);
			// Load_RawFileAsset((RawFile **)varRawFilePtr);

			if (v1)
				*v1 = *(const void **)varRawFilePtr;
		}
	}

	DB_PopStreamPos();
}

void Load_RawFile(bool atStreamStart)
{
	Load_Stream(atStreamStart, varRawFile, sizeof(RawFile));
	DB_PushStreamPos(4);

	varXString = varRawFile;
	Load_XString(0);

	if (varRawFile->buffer)
	{
		varRawFile->buffer = (const char *)AllocLoad_raw_byte();
		varConstChar = (const char *)varRawFile->buffer;
		Load_ConstCharArray(1, varRawFile->len + 1);
	}

	DB_PopStreamPos();
}

void Load_XString(bool atStreamStart)
{
	Load_Stream(atStreamStart, varXString, 4);
	if (*(DWORD *)varXString)
	{
		if (*(DWORD *)varXString == -1)
		{
			*(DWORD *)varXString = (DWORD)AllocLoad_raw_byte();
			varConstChar = *(const char **)varXString;
			Load_XStringCustom((const char **)varXString);
		}
		else
		{
			DB_ConvertOffsetToPointer(varXString);
		}
	}
}

void Load_ConstCharArray(bool atStreamStart, int count)
{
	Load_Stream(atStreamStart, varConstChar, count * sizeof(char));
}

void Load_XAssetHeader(bool atStreamStart)
{
#define LOAD_ASSET(Type, Name) case Type: var##Name = varXAssetHeader; Load_##Name(atStreamStart); break;

	switch (varXAsset->type)
	{
		LOAD_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSetPtr);
		LOAD_ASSET(ASSET_TYPE_IMAGE, GfxImagePtr);

	case 36:
		varRawFilePtr = varXAssetHeader;
		Load_RawFilePtr(atStreamStart);
		break;

	//default:
	//	Load_RandomPointer(atStreamStart);
	//	break;
	}

#undef LOAD_ASSET
}