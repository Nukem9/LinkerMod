#pragma once

struct SourceBufferInfo
{
	const char *codePos;
	char *buf;
	const char *sourceBuf;
	int len;
	int sortedIndex;
	bool archive;
};

struct SaveSourceBufferInfo
{
	char *sourceBuf;
	int len;
};

struct SourceLookup
{
	unsigned int sourcePos;
	int type;
};

struct OpcodeLookup
{
	const char *codePos;
	unsigned int sourcePosIndex;
	unsigned int sourcePosCount;
};

struct scrParserGlob_t
{
	OpcodeLookup *opcodeLookup;					// 0x3268380
	unsigned int opcodeLookupMaxLen;			// ?
	unsigned int opcodeLookupLen;				// 0x3268388
	SourceLookup *sourcePosLookup;				// 0x326838C
	char _pad1[0x14];
	SaveSourceBufferInfo *saveSourceBufferLookup;// 0x32683A4
	char _pad2[0xC];
};
STATIC_ASSERT_SIZE(scrParserGlob_t, 0x34);

struct scrParserPub_t
{
	SourceBufferInfo *sourceBufferLookup;		// 0x32683EC
	unsigned int sourceBufferLookupLen;			// 0x32683F0
	const char *scriptfilename;					// 0x32683F4
	const char *sourceBuf;						// 0x32683F8
};
STATIC_ASSERT_SIZE(scrParserPub_t, 0x10);

static scrParserGlob_t *gScrParserGlob = (scrParserGlob_t *)0x03268380;
static scrParserPub_t *gScrParserPub = (scrParserPub_t *)0x032683EC;

typedef char *(__cdecl* Scr_ReadFile_t)(scriptInstance_t inst, const char *filename, const char *extFilename, const char *codePos, bool archive);
static Scr_ReadFile_t Scr_ReadFile_FastFile = (Scr_ReadFile_t)0x008A57B0;
static Scr_ReadFile_t Scr_ReadFile_LoadObj = (Scr_ReadFile_t)0x008A58C0;

void hk_Scr_ReadFile();//scriptInstance_t inst, const char *filename, const char *codePos, bool archive);

OpcodeLookup *Scr_GetPrevSourcePosOpcodeLookup(scriptInstance_t inst, const char *codePos);
unsigned int Scr_GetPrevSourcePos(scriptInstance_t inst, const char *codePos, unsigned int index);
int Scr_GetLineNumInternal(const char *buf, unsigned int sourcePos, const char **startLine, unsigned int *col, SourceBufferInfo *binfo);
int Scr_GetLineNum(scriptInstance_t inst, unsigned int bufferIndex, unsigned int sourcePos);
char *Scr_ReadFile(scriptInstance_t inst, const char *filename, const char *extFilename, const char *codePos, bool archive);
void Scr_CopyFormattedLine(char *line, const char *rawLine);
int Scr_GetLineInfo(const char *buf, unsigned int sourcePos, unsigned int *col, char *line, SourceBufferInfo *binfo);
void Scr_PrintSourcePos(scriptInstance_t inst, int channel, const char *filename, const char *buf, unsigned int sourcePos);
unsigned int Scr_GetSourceBuffer(scriptInstance_t inst, const char *codePos);
void Scr_PrintPrevCodePos(scriptInstance_t inst, int channel, const char *codePos, unsigned int index);
void RuntimeErrorInternal(scriptInstance_t inst, int channel, const char *codePos, unsigned int index, const char *msg);
void RuntimeError(scriptInstance_t inst, const char *codePos, unsigned int index, const char *msg, const char *dialogMessage);