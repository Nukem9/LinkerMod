#include "stdafx.h"
#include "jsoncpp\include\json\json.h"

#define ARENA_FILE_MAX_SIZE 8192

sharedUiInfo_t * const sharedUiInfo = (sharedUiInfo_t * const)0x025760F0;

int& ui_numArenas = *(int*)0x025F6940;
char** ui_arenaInfos = *(char*(*)[128])0x025F6740;

//
// Needed because the Detours lib can't handle relocating relative calls
//
void __declspec(naked) UI_OpenMenu_f_o(void)
{
	static DWORD UI_GetInfo = 0x0046BE80;
	static DWORD dwRetn = 0x00847219;

	_asm
	{
		push esi
		push edi
		push 0
		call UI_GetInfo
		jmp dwRetn
	}
}

void __cdecl UI_OpenMenu_f(void)
{
	if (sv_mapname && sv_mapname->current.string && cl_ingame && cl_ingame->current.enabled)
	{
		if (strcmp(sv_mapname->current.string, "frontend") == 0)
			UI_OpenMenu_f_o();
	}
}

bool __cdecl UI_LoadModArenas()
{
	int fileHandle = 0;
	size_t fileSize = FS_FOpenFileRead("mod.arena", &fileHandle);

	if (!fileHandle)
	{
		Com_PrintWarning(13, "Customized arena file not found: %s\n", "mod.arena");
		return false;
	}

	if (!fileSize)
	{
		Com_PrintWarning(13, "Customized arena file is empty: %s\n", "mod.arena");
		FS_FCloseFile(fileHandle); //Fix for leaked handles
		return false;
	}

	if (fileSize > ARENA_FILE_MAX_SIZE)
	{
		Com_PrintWarning(13, "Customized arena file size is too big to load > %d: %s\n", ARENA_FILE_MAX_SIZE, "mod.arena");
		FS_FCloseFile(fileHandle); //Fix for leaked handles
		return false;
	}

	char buf[ARENA_FILE_MAX_SIZE];
	FS_Read(buf, fileSize, fileHandle);
	FS_FCloseFile(fileHandle);

	ui_numArenas = UI_ParseInfos(buf, 128 - ui_numArenas, &ui_arenaInfos[ui_numArenas]);
	return true;
}

const char* __cdecl UI_SelectedMap_LoadName(int index, int* actual)
{
	*actual = 0;

	for (int i = 0, c = 0; i < sharedUiInfo->mapCount; i++)
	{
		if (sharedUiInfo->mapList[i].active)
		{
			if (c == index)
			{
				*actual = i;
				return UI_SafeTranslateString(sharedUiInfo->mapList[i].mapLoadName);
			}

			c++;
		}
	}

	return "";
}

void* rtn_UI_FeederSelection_AllMaps = (void*)0x00835304;
void __declspec(naked) mfh_UI_FeederSelection_AllMaps()
{
	_asm
	{
		call UI_SelectedMap_LoadName
		add esp, 8

		pushad
		push eax
		push 0x00A2869C
		call Dvar_SetStringByName
		add esp, 8
		popad

		movss   xmm0, [esp + 0x10]
		jmp rtn_UI_FeederSelection_AllMaps
	}
}

bool __cdecl GetOperand(OperandStack *dataStack, Operand *data)
{
	bool result = false;

	_asm
	{
		mov edi, data
		mov esi, dataStack
		mov eax, 0x00851930
		call eax
		mov result, al
	}

	return result;
}

const char* UI_GetModInfo_JSon_GetString(const char* str, const char* key, char* buf, unsigned int buf_len)
{
	memset(buf, 0, buf_len);

	Json::Reader reader;
	Json::Value root;

	const char* nullstr = "";
#if _DEBUG
	const bool isDebug = true;
	#else
	const bool isDebug = false;
#endif

	if (!reader.parse(str, root))
		return isDebug ? "Couldn't parse JSon" : nullstr;
	
	if (!root.isObject())
		return isDebug ? "Root isn't an object!" : nullstr;

	if (!root.isMember(key))
		return isDebug ? "Can't find key" : nullstr;

	if (!root[key].isString())
		return isDebug ? "Key does not refer to a string type" : nullstr;;

	std::string tmp = root[key].asString(); // Ensures the string data isn't destroyed before strcpy runs
	strcpy_s(buf, buf_len, tmp.c_str());
	return  buf;
}

const char* UI_GetModInfo_GetDescription(const char* str)
{
	static char desc[MODDESC_LEN];
	return UI_GetModInfo_JSon_GetString(str , "desc", desc, ARRAYSIZE(desc));
}

const char* UI_GetModInfo_GetCompatibilityInfo(const char* str)
{
	const char* nullstr = "";

	char versionString[32];
	const char* data = UI_GetModInfo_JSon_GetString(str, "comp", versionString, ARRAYSIZE(versionString));
	if (!*data)
		return nullstr;

	static char compInfo[MAX_PATH];
	compInfo[0] = '\0';

	Version_t dep_ver(data);
	if (!semver::v2::IsCompatibleVersion(dep_ver, (Version_t&)DLL_Version()))
	{
		sprintf_s(compInfo, "^1Requires Game_Mod %d.%d.%d+\n",
			dep_ver.GetMajorVersion(),
			dep_ver.GetMinorVersion(),
			dep_ver.GetPatchVersion());
		return compInfo;
	}

	return nullstr;
}

void __cdecl UI_GetModInfo(const int localClientNum, struct itemDef_s *item, OperandStack *dataStack)
{
	const char *str = "GetModInfo unhandled";

	Operand operand;
	GetOperand(dataStack, &operand);
	const char* src = GetSourceString(operand);
	if (_stricmp(src, "modName") == 0)
		str = sharedUiInfo_modList[sharedUiInfo_modIndex].modName;
	else if (_stricmp(src, "modDescr") == 0)
	{
		str = sharedUiInfo_modList[sharedUiInfo_modIndex].modDescr;
		if (str != NULL)
		{
			switch (*str)
			{
			case DESC_DESC:
				str++;
				break;
			case DESC_JSON:
				str = UI_GetModInfo_GetDescription(str + 1);
				break;
			default:
				str = "";
				break;
			}
		}
	}
	else if (_stricmp(src, "modCompat") == 0)
	{
		str = sharedUiInfo_modList[sharedUiInfo_modIndex].modDescr;
		if (str != NULL)
		{
			switch (*str)
			{
			case DESC_JSON:
				str = UI_GetModInfo_GetCompatibilityInfo(str + 1);
				break;
			case DESC_DESC:
				// We just assume that legacy mods are compatible for now
			default:
				str = "";
				break;
			}
		}
	}

	if (uiscript_debug && uiscript_debug->current.integer)
		Expression_TraceInternal("GetModInfo() = %s\n", str);

	int index = dataStack->numOperandLists;
	dataStack->stack[index].operands[0].dataType = VAL_STRING;
	dataStack->stack[index].operands[0].internals.string = str;
	dataStack->stack[dataStack->numOperandLists++].operandCount = 1;
}