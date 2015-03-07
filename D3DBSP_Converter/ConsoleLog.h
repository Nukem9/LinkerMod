#include <WTypes.h>
#include <iostream>

enum LOGFILE_MODE
{
	LOGFILE_READ_ONLY,
	LOGFILE_OVERWRITE,
	LOGFILE_APPEND,
	LOGFILE_UPDATE,
	LOGFILE_UPDATE_OVERWRITE,
	LOGFILE_UPDATE_APPEND
};

int Con_Init(void);
int Con_Init(const char* logfilePath, LOGFILE_MODE mode);

int Log_Init(const char* logfilePath, LOGFILE_MODE mode);

int Con_Printf(const char* fmt, ...);
int Con_Warning(const char* fmt, ...);
int Con_Error(const char* fmt, ...);

int Log_Printf(const char* fmt, ...);

DWORD Con_ErrorCount(void);
DWORD Con_WarningCount(void);

int Con_Restore(void);