#pragma once
#include <WTypes.h>
#include "db_registry.h"

enum PROCESS_TYPE
{
	PROCESS_UNKNOWN,
	PROCESS_BLACK_OPS,
	PROCESS_BLACK_OPS_MP,
};

PROCESS_TYPE IsSupportedProcess(WCHAR* processString);

DWORD GetSupportedProcess();