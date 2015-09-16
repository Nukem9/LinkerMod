#pragma once

#define STATS_BUFFER_SIZE 40960

enum taskCompleteResults
{
	TASK_NOTCOMPLETE = 0x0,
	TASK_COMPLETE = 0x1,
	TASK_ERROR = 0x2,
};

struct dwFileTask
{
	char _pad[0xE0];
};

struct fileRetryInfo
{
	int lastAttemptTime;
	int lastAttemptInterval;
	int retryCount;
};

struct dwFileOperationInfo
{
	dwFileTask fileTask;
	fileRetryInfo retryInfo;
	int taskType;
	bool fetchCompleted;
	bool writeCompleted;
	bool isUserFile;
	bool isCompressedFile;
	const char *menuDef;
	char *fileBuffer;
	int bufferSize;
	void(__cdecl *fileOperationSucessFunction)(const int, void *);
	taskCompleteResults(__cdecl *fileNotFoundFunction)(const int, void *);
	void(__cdecl *fileOperationFailureFunction)(const int, void *);
	unsigned __int64 ownerID;
	bool *alreadyUploaded;
};

enum statsLocation
{
	STATS_LOCATION_NORMAL = 0x0,
	STATS_LOCATION_FORCE_NORMAL = 0x1,
	STATS_LOCATION_BACKUP = 0x2,
	STATS_LOCATION_STABLE = 0x3,
	STATS_LOCATION_OTHERPLAYER = 0x4,
	STATS_LOCATION_BASICTRAINING = 0x5,
	STATS_LOCATION_GLOBAL = 0x6,
	STATS_LOCATION_GLOBALSTABLE = 0x7,
	STATS_LOCATION_COUNT = 0x8,
};

struct persistentStats
{
	char statsBuffer[STATS_BUFFER_SIZE];
	bool isChecksumValid;
	bool statsWriteNeeded;
	bool statsValidatedWithDDL;
	bool statsFetched;
};

struct playerNetworkData
{
	persistentStats playerStats;
	persistentStats playerStatsBackup;
	persistentStats stableStatsBuffer;
	persistentStats basicTrainingStats;
	persistentStats globalplayerStats;
	persistentStats globalStablePlayerStats;
	bool firstTimeRunning;
	bool fileShareFetched;
	void *fileOps;
};

typedef void TaskRecord;
typedef void TaskDefinition;

void Patch_DDL();

void LiveStorage_VerifyCorrectStats(persistentStats *stats, statsLocation location);
persistentStats *LiveStorage_GetPersStatsBuffer(const int controllerIndex, statsLocation playerStatsLocation, bool verifyLocation);
int LiveStorage_GetStatsBufferSize(const int controllerIndex, statsLocation playerStatsLocation, bool verifyLocation);
char *LiveStorage_GetStatsBuffer(const int controllerIndex, statsLocation playerStatsLocation, bool verifyLocation);
void LiveStorage_ClearPlayerStats(const int controllerIndex);
bool LiveStorage_DoWeHaveCurrentStats(const int controllerIndex);
void LiveStorage_SetStatsChecksumValid(const int controllerIndex, statsLocation playerStatsLocation, bool isValid);
bool LiveStorage_GetStatsChecksumValid(const int controllerIndex, statsLocation playerStatsLocation);
void LiveStorage_SetStatsDDLValidated(const int controllerIndex, statsLocation playerStatsLocation, bool statsValidatedWithDDL);
bool LiveStorage_AreStatsDDLValidated(const int controllerIndex, statsLocation playerStatsLocation);
void LiveStorage_SetStatsFetched(const int localControllerIndex, statsLocation playerStatsLocation, bool isFetched);
bool LiveStorage_DoWeHaveStats(const int controllerIndex, statsLocation playerStatsLocation);

TaskRecord *LiveStorage_ReadCommonStats(const int controllerIndex, bool silent, statsLocation location, TaskDefinition *taskDef, dwFileOperationInfo *fileInfo);