#include <WTypes.h>
#include <iostream>

static HANDLE g_hProcess = NULL;

BYTE* db_hashTable = nullptr;
void* g_assetEntryPool = nullptr;
char* g_zoneNames = nullptr;
