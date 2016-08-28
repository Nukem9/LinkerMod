#pragma once

#include "ff.h"
#include "arg.h"
#include "files.h"
#include "io.h"
#include <Windows.h>
#include "AppInfo.h"
#include "zlib/zlib.h"

#define DEFLATE_BUFFER_SIZE 32768
#define DBFILE_BUFFER_SIZE 524288

struct StreamPosInfo
{
	char *pos;
	unsigned int index;
};

struct XBlock
{
	char *data;
	unsigned int size;
};

struct DB_LoadData
{
	void *f;
	const char *filename;
	XBlock *blocks;
	int flags;
	int outstandingReads;
	OVERLAPPED overlapped;
	z_stream_s stream;
	char *compressBufferStart;
	char *compressBufferEnd;
	void(__cdecl *interrupt)();
	int allocType;
	bool abort;
	int startTime;
	int deflateBufferPos;
	int deflateRemainingFileSize;
	char deflateBuffer[DEFLATE_BUFFER_SIZE];
};

enum XAssetType
{
	ASSET_TYPE_XMODELPIECES = 0x0,
	ASSET_TYPE_PHYSPRESET = 0x1,
	ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
	ASSET_TYPE_DESTRUCTIBLEDEF = 0x3,
	ASSET_TYPE_XANIMPARTS = 0x4,
	ASSET_TYPE_XMODEL = 0x5,
	ASSET_TYPE_MATERIAL = 0x6,
	ASSET_TYPE_TECHNIQUE_SET = 0x7,
	ASSET_TYPE_IMAGE = 0x8,
	ASSET_TYPE_SOUND = 0x9,
	ASSET_TYPE_SOUND_PATCH = 0xA,
	ASSET_TYPE_CLIPMAP = 0xB,
	ASSET_TYPE_CLIPMAP_PVS = 0xC,
	ASSET_TYPE_COMWORLD = 0xD,
	ASSET_TYPE_GAMEWORLD_SP = 0xE,
	ASSET_TYPE_GAMEWORLD_MP = 0xF,
	ASSET_TYPE_MAP_ENTS = 0x10,
	ASSET_TYPE_GFXWORLD = 0x11,
	ASSET_TYPE_LIGHT_DEF = 0x12,
	ASSET_TYPE_UI_MAP = 0x13,
	ASSET_TYPE_FONT = 0x14,
	ASSET_TYPE_MENULIST = 0x15,
	ASSET_TYPE_MENU = 0x16,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x17,
	ASSET_TYPE_WEAPON = 0x18,
	ASSET_TYPE_WEAPONDEF = 0x19,
	ASSET_TYPE_WEAPON_VARIANT = 0x1A,
	ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1B,
	ASSET_TYPE_FX = 0x1C,
	ASSET_TYPE_IMPACT_FX = 0x1D,
	ASSET_TYPE_AITYPE = 0x1E,
	ASSET_TYPE_MPTYPE = 0x1F,
	ASSET_TYPE_MPBODY = 0x20,
	ASSET_TYPE_MPHEAD = 0x21,
	ASSET_TYPE_CHARACTER = 0x22,
	ASSET_TYPE_XMODELALIAS = 0x23,
	ASSET_TYPE_RAWFILE = 0x24,
	ASSET_TYPE_STRINGTABLE = 0x25,
	ASSET_TYPE_PACK_INDEX = 0x26,
	ASSET_TYPE_XGLOBALS = 0x27,
	ASSET_TYPE_DDL = 0x28,
	ASSET_TYPE_GLASSES = 0x29,
	ASSET_TYPE_EMBLEMSET = 0x2A,
	ASSET_TYPE_COUNT = 0x2B,
	ASSET_TYPE_STRING = 0x2B,
	ASSET_TYPE_ASSETLIST = 0x2C,
};

void TestIt(char *ffData, size_t size, HANDLE file);

int DB_ReadData();
void DB_ReadXFileStage();
void DB_LoadXFileSetSize(int size);
bool DB_LoadXFile(void *f, const char *filename, XBlock *blocks, void(__cdecl *interrupt)(), char *buf, int allocType, int flags);
void DB_LoadXFileData(char *pos, int size);
void DB_LoadXFileInternal();
int Load_DelayStream();
void DB_AllocXBlocks(unsigned int *blockSize, XBlock *blocks);
void DB_InitStreams(XBlock *blocks);
void Load_Stream(bool atStreamStart, const void *ptr, int size);
void Load_XStringCustom(const char **str);
void Load_TempStringCustom(const char **str);
void Load_TempString(bool atStreamStart);
void Load_TempStringArray(bool atStreamStart, int count);
void Load_ScriptStringList(bool atStreamStart);
void Load_XAssetListCustom();
void DB_SetStreamIndex(unsigned int index);
void DB_IncStreamPos(int size);
char *DB_AllocStreamPos(int alignment);
char *DB_GetStreamPos();
void DB_PushStreamPos(unsigned int index);
void DB_PopStreamPos();
const void **DB_InsertPointer();
void DB_ConvertOffsetToAlias(void *data);
void DB_ConvertOffsetToPointer(void *data);
int DB_LoadXFileDataNullTerminated(char *pos);
XAsset *AllocLoad_Align3();
char *AllocLoad_raw_byte();
void Load_XAssetArrayCustom(int count);
void Load_XAsset(bool atStreamStart);
void Load_XAssetHeader(bool atStreamStart);

void Load_RawFilePtr(bool atStreamStart);
void Load_RawFile(bool atStreamStart);
void Load_XString(bool atStreamStart);
void Load_ConstCharArray(bool atStreamStart, int count);

//////////////////////////////////////
// ----- ASSET_TYPE_MATERIAL ----- //
//////////////////////////////////////
void MaterialHandle(bool atStreamStart);
void Load_Material(bool atStreamStart);
void Load_MaterialInfo(bool atStreamStart);

//////////////////////////////////////
// --- ASSET_TYPE_TECHNIQUE_SET --- //
//////////////////////////////////////
void Load_MaterialTechniqueSetPtr(bool atStreamStart);
void Load_MaterialTechniqueSet(bool atStreamStart);
void Load_MaterialTechniquePtrArray(bool atStreamStart, int count);
void Load_MaterialTechniquePtr(bool atStreamStart);
void Load_MaterialTechnique(bool atStreamStart);
void Load_MaterialPassArray(bool atStreamStart, int count);
void Load_MaterialPass(bool atStreamStart);
void Load_MaterialVertexDeclaration(bool atStreamStart);
void Load_MaterialVertexShaderPtr(bool atStreamStart);
void Load_MaterialVertexShader(bool atStreamStart);
void Load_MaterialVertexShaderProgram(bool atStreamStart);
void Load_GfxVertexShaderLoadDef(bool atStreamStart);
void Load_MaterialPixelShaderPtr(bool atStreamStart);
void Load_MaterialPixelShader(bool atStreamStart);
void Load_MaterialPixelShaderProgram(bool atStreamStart);
void Load_GfxPixelShaderLoadDef(bool atStreamStart);
void Load_MaterialShaderArgumentArray(bool atStreamStart, int count);
void Load_MaterialShaderArgument(bool atStreamStart);
void Load_MaterialArgumentDef(bool atStreamStart);
void Load_MaterialArgumentCodeConst(bool atStreamStart);

//////////////////////////////////////
// ------- ASSET_TYPE_IMAGE ------- //
//////////////////////////////////////
void Load_GfxImagePtr(bool atStreamStart);
void Load_GfxImage(bool atStreamStart);
void Load_GfxTextureLoad(bool atStreamStart);
void Load_GfxImageLoadDef(bool atStreamStart);