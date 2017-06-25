#pragma once

enum sysEventType_t
{
	SE_NONE = 0x0,
	SE_KEY = 0x1,
	SE_CHAR = 0x2,
	SE_CONSOLE = 0x3,
};

struct sysEvent_t
{
	int evTime;
	sysEventType_t evType;
	int evValue;
	int evValue2;
	int evPtrLength;
	void *evPtr;
};

int Sys_Milliseconds();
void Sys_GetEvent(sysEvent_t *event);
void Sys_NormalExit();
void Sys_OutOfMemErrorInternal(const char *filename, int line);