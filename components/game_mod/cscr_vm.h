#pragma once

union VariableUnion
{
	int intValue;
	float floatValue;
	unsigned int stringValue;
	const float *vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
	struct VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};

struct VariableValue
{
	VariableUnion u;
	int type;
};

struct function_stack_t
{
	const char *pos;
	unsigned int localId;
	unsigned int localVarCount;
	VariableValue *top;
	VariableValue *startTop;
};

struct function_frame_t
{
	function_stack_t fs;
	int topType;
};
STATIC_ASSERT_SIZE(function_frame_t, 0x18);

struct scrVmGlob_t
{
};

struct scrVmPub_t
{
	unsigned int *localVars;					// 0x32C8738
	char _pad1[0x4];
	int function_count;							// 0x32C8740
	char _pad2[0x8];
	bool debugCode;								// 0x32C874C
	bool abort_on_error;						// 0x32C874D
	bool terminal_error;						// 0x32C874E
	char _pad3[0x9];
	function_frame_t function_frame_start[32];	// 0x32C8758
	char _pad4[0x4000];
};
STATIC_ASSERT_SIZE(scrVmPub_t, 0x4320);

static scrVmPub_t *gScrVmPub = (scrVmPub_t *)0x032C8738;

const char *Scr_GetString(unsigned int index, scriptInstance_t inst);
void Scr_AddBool(int value, scriptInstance_t inst);
void Scr_Error(const char *error, bool force_terminal);
void Scr_ObjectError(const char *error, scriptInstance_t inst);
void SL_RemoveRefToString(scriptInstance_t inst, unsigned int stringValue);

typedef void (__cdecl* GScr_NewHudElem_t)();
static GScr_NewHudElem_t GScr_NewHudElem = (GScr_NewHudElem_t)0x006707C0;

void __cdecl GScr_NewDebugHudElem();