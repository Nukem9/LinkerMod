#include "stdafx.h"

playerNetworkData controllerNetworkData[2];
persistentStats s_otherPlayerStats;

char s_tempStatsBuffer[STATS_BUFFER_SIZE];
char s_tempGlobalStatsBuffer[STATS_BUFFER_SIZE];

char *newWAD = "online_tu14_mp_";

char s_playlistStringBuffer[98304];

void Patch_DDL()
{
	DWORD newStatsSize = LiveStorage_GetStatsBufferSize(0, STATS_LOCATION_NORMAL, false);
	DWORD newGlobalBuf = (DWORD)&s_tempGlobalStatsBuffer;
	DWORD newTempBuf = (DWORD)&s_tempStatsBuffer;

	PatchMemory(0x0098DB6A, (PBYTE)&newGlobalBuf, 4);	// LiveStorage_GetGlobalBlobSuccess (memcpy)
	PatchMemory(0x0098DABB, (PBYTE)&newGlobalBuf, 4);	// LiveStorage_ReadPlayerGlobalBlob
	PatchMemory(0x0098DAC8, (PBYTE)&newStatsSize, 4);	// LiveStorage_ReadPlayerGlobalBlob

	PatchMemory(0x009821EB, (PBYTE)&newTempBuf, 4);		// LiveStorage_ReadPlayerStatsSuccessful
	PatchMemory(0x009821E6, (PBYTE)&newStatsSize, 4);	// LiveStorage_ReadPlayerStatsSuccessful

	PatchMemory(0x0098032B, (PBYTE)"\xCC", 1);			// LiveStorage_ValidateWithDDL
	PatchMemory(0x00980386, (PBYTE)"\xCC", 1);			// LiveStorage_ValidateWithDDL
	//PatchMemory(0x0098032C, (PBYTE)&newStatsSize, 4);
	//PatchMemory(0x00980387, (PBYTE)&newStatsSize, 4);

	PatchMemory(0x0098AA79, (PBYTE)&newStatsSize, 4);	// SV_IsStatsBlobOK
	PatchMemory(0x0098AA96, (PBYTE)"\xCC", 1);			// SV_IsStatsBlobOK
	PatchMemory(0x0098AAD9, (PBYTE)"\xCC", 1);			// SV_IsStatsBlobOK

	PatchMemory(0x0098AD6B, (PBYTE)&newStatsSize, 4);	// SV_DWReadClientCAC

	Detours::X86::DetourFunction((PBYTE)0x009801D0, (PBYTE)&LiveStorage_GetStatsBufferSize);
	Detours::X86::DetourFunction((PBYTE)0x0097FF20, (PBYTE)&LiveStorage_GetPersStatsBuffer);
	Detours::X86::DetourFunction((PBYTE)0x00986950, (PBYTE)&LiveStorage_ClearPlayerStats);
	Detours::X86::DetourFunction((PBYTE)0x00980200, (PBYTE)&LiveStorage_SetStatsChecksumValid);
	Detours::X86::DetourFunction((PBYTE)0x009801E0, (PBYTE)&LiveStorage_GetStatsChecksumValid);
	Detours::X86::DetourFunction((PBYTE)0x00980420, (PBYTE)&LiveStorage_SetStatsDDLValidated);
	Detours::X86::DetourFunction((PBYTE)0x00980540, (PBYTE)&LiveStorage_SetStatsFetched);
	Detours::X86::DetourFunction((PBYTE)0x00982440, (PBYTE)&LiveStorage_ReadCommonStats);
	Detours::X86::DetourFunction((PBYTE)0x0097FF00, (PBYTE)&LiveStorage_GetStatsBuffer);
	Detours::X86::DetourFunction((PBYTE)0x009804A0, (PBYTE)&LiveStorage_DoWeHaveStats);
	Detours::X86::DetourFunction((PBYTE)0x00980400, (PBYTE)&LiveStorage_AreStatsDDLValidated);
	Detours::X86::DetourFunction((PBYTE)0x009804C0, (PBYTE)&LiveStorage_DoWeHaveCurrentStats);

	PatchMemory(0x0098759A, (PBYTE)&newWAD, 4);
	PatchMemory(0x00987744, (PBYTE)&newWAD, 4);

	DWORD strBuf = (DWORD)&s_playlistStringBuffer;
	DWORD strSize = sizeof(s_playlistStringBuffer);
	PatchMemory(0x0075746C, (PBYTE)&strBuf, 4);
	PatchMemory(0x0075737C, (PBYTE)&strBuf, 4);

	PatchMemory(0x00757386, (PBYTE)&strSize, 4);
	PatchMemory(0x007573B5, (PBYTE)&strSize, 4);

	PatchMemory(0x00757476, (PBYTE)&strSize, 4);
	PatchMemory(0x007574A5, (PBYTE)&strSize, 4);

}

void LiveStorage_VerifyCorrectStats(persistentStats *stats, statsLocation location)
{
	((void(__cdecl *)(persistentStats *, statsLocation))0x00980080)(stats, location);
}

TaskRecord *LiveStorage_ReadDWFile(const int controllerIndex, dwFileOperationInfo *fileInfo)
{
	return ((TaskRecord *(__cdecl *)(const int, dwFileOperationInfo *))0x00981090)(controllerIndex, fileInfo);
}

TaskRecord *LiveStorage_SetupNestedTask(TaskDefinition *taskdef, int controllerIndex, TaskRecord *nestedTask, void *payload)
{
	return ((TaskRecord *(__cdecl *)(TaskDefinition *, int, TaskRecord *, void *))0x009805E0)(taskdef, controllerIndex, nestedTask, payload);
}

bool LiveStorage_DoWeHaveCurrentStats(const int controllerIndex)
{
	return LiveStorage_GetPersStatsBuffer(controllerIndex, STATS_LOCATION_NORMAL, false)->statsFetched;
}

persistentStats *LiveStorage_GetPersStatsBuffer(const int controllerIndex, statsLocation playerStatsLocation, bool verifyLocation)
{
	persistentStats *stats = nullptr;

	switch (playerStatsLocation)
	{
	case STATS_LOCATION_NORMAL:
	case STATS_LOCATION_FORCE_NORMAL:
		stats = &controllerNetworkData[controllerIndex].playerStats;
		break;
	case STATS_LOCATION_BACKUP:
		stats = &controllerNetworkData[controllerIndex].playerStatsBackup;
		break;
	case STATS_LOCATION_STABLE:
		stats = &controllerNetworkData[controllerIndex].stableStatsBuffer;
		break;
	case STATS_LOCATION_OTHERPLAYER:
		stats = &s_otherPlayerStats;
		break;
	case STATS_LOCATION_BASICTRAINING:
		stats = &controllerNetworkData[controllerIndex].basicTrainingStats;
		break;
	case STATS_LOCATION_GLOBAL:
		stats = &controllerNetworkData[controllerIndex].globalplayerStats;
		break;
	case STATS_LOCATION_GLOBALSTABLE:
		stats = &controllerNetworkData[controllerIndex].globalStablePlayerStats;
		break;
	}

	/*if ((playerStatsLocation == STATS_LOCATION_NORMAL || playerStatsLocation == STATS_LOCATION_GLOBAL)
	&& xblive_basictraining->current.enabled)
	stats = controllerNetworkData[controllerIndex].basicTrainingStats.statsBuffer;
	*/

	if (verifyLocation)
		LiveStorage_VerifyCorrectStats(stats, playerStatsLocation);

	return stats;
}

int LiveStorage_GetStatsBufferSize(const int controllerIndex, statsLocation playerStatsLocation, bool verifyLocation)
{
	static_assert(sizeof(persistentStats) == 40964, "Invalid persistentStats size!");
	static_assert(STATS_BUFFER_SIZE == 40960, "Invalid STATS_BUFFER_SIZE size! (Does not match game)");

	return STATS_BUFFER_SIZE;
}

char *LiveStorage_GetStatsBuffer(const int controllerIndex, statsLocation playerStatsLocation, bool verifyLocation)
{
	return (char *)LiveStorage_GetPersStatsBuffer(controllerIndex, playerStatsLocation, verifyLocation);
}

void LiveStorage_ClearPlayerStats(const int controllerIndex)
{
	memset((LPVOID)0x0A56DA38, 0, 241040);
	memset(&controllerNetworkData, 0, sizeof(controllerNetworkData));

	*(void **)(0xA5A87C4 + (0x3AD90 * controllerIndex)) = (void *)(0x0A5BD908 + (0x6F58 * controllerIndex));
	controllerNetworkData[controllerIndex].fileOps = (void *)(0x0A5BD908 + (0x6F58 * controllerIndex));//&controllerFileOps[controllerIndex];
}

void LiveStorage_SetStatsChecksumValid(const int controllerIndex, statsLocation playerStatsLocation, bool isValid)
{
	LiveStorage_GetPersStatsBuffer(controllerIndex, playerStatsLocation, true)->isChecksumValid = isValid;
}

bool LiveStorage_GetStatsChecksumValid(const int controllerIndex, statsLocation playerStatsLocation)
{
	return LiveStorage_GetPersStatsBuffer(controllerIndex, playerStatsLocation, true)->isChecksumValid;
}

void LiveStorage_SetStatsDDLValidated(const int controllerIndex, statsLocation playerStatsLocation, bool statsValidatedWithDDL)
{
	LiveStorage_GetPersStatsBuffer(controllerIndex, playerStatsLocation, true)->statsValidatedWithDDL = statsValidatedWithDDL;
}

bool LiveStorage_AreStatsDDLValidated(const int controllerIndex, statsLocation playerStatsLocation)
{
	return LiveStorage_GetPersStatsBuffer(controllerIndex, playerStatsLocation, true)->statsValidatedWithDDL;
}

void LiveStorage_SetStatsFetched(const int localControllerIndex, statsLocation playerStatsLocation, bool isFetched)
{
	LiveStorage_GetPersStatsBuffer(localControllerIndex, playerStatsLocation, true)->statsFetched = isFetched;
}

bool LiveStorage_DoWeHaveStats(const int controllerIndex, statsLocation playerStatsLocation)
{
	return LiveStorage_GetPersStatsBuffer(controllerIndex, playerStatsLocation, true)->statsFetched;
}

TaskRecord *LiveStorage_ReadCommonStats(const int controllerIndex, bool silent, statsLocation location, TaskDefinition *taskDef, dwFileOperationInfo *fileInfo)
{
	fileInfo->retryInfo.lastAttemptTime = 0;
	fileInfo->retryInfo.lastAttemptInterval = 0;
	fileInfo->retryInfo.retryCount = 0;
	fileInfo->isUserFile = true;
	fileInfo->isCompressedFile = true;

	*(BYTE *)(fileInfo + 220) = 1;
	//fileInfo->fileTask.m_optional = 1;

	fileInfo->fileBuffer = LiveStorage_GetStatsBuffer(controllerIndex, location, true);
	fileInfo->bufferSize = LiveStorage_GetStatsBufferSize(controllerIndex, location, true);
	fileInfo->menuDef = "popup_fetchstats";

	if (location != STATS_LOCATION_BASICTRAINING)
	{
		memset(s_tempStatsBuffer, 0, sizeof(s_tempStatsBuffer));
		fileInfo->fileBuffer = s_tempStatsBuffer;
		fileInfo->bufferSize = sizeof(s_tempStatsBuffer);
	}

	return LiveStorage_SetupNestedTask(taskDef, controllerIndex, LiveStorage_ReadDWFile(controllerIndex, fileInfo), fileInfo);
}