#include "stdafx.h"

int Sys_Milliseconds()
{
	return ((int (__cdecl *)())0x0062EF60)();
}

void Sys_GetEvent(sysEvent_t *event)
{
	((void(__cdecl *)(sysEvent_t *))0x0068BB40)(event);
}

void Sys_NormalExit()
{
	((void(__cdecl *)())0x00553790)();
}

void Sys_OutOfMemErrorInternal(const char *filename, int line)
{
	Sys_EnterCriticalSection(CRITSECT_FATAL_ERROR);
	Com_Printf(16, "Out of memory: filename '%s', line %d\n", filename, line);

	const char* title = Win_LocalizeRef("WIN_OUT_OF_MEM_TITLE");
	const char* body = Win_LocalizeRef("WIN_OUT_OF_MEM_BODY");
	HWND hWnd = GetActiveWindow();

	char msg[EXCEPTION_STR_MAXLEN];
	sprintf_s(msg, "%s\n\n", body);

	void* frame[EXCEPTION_STACKTRACE_MAXFRAMECOUNT];
	int frameCount = CaptureStackBackTrace(0, ARRAYSIZE(frame), frame, nullptr);

	if (frameCount != 0)
		strcat_s(msg, "Stack Trace:\n");

	for (int i = 0; i < frameCount; i++)
	{
		char buf[256];
		sprintf_s(buf, "frame[%s%d]: %0*p%s", i < 10 ? " " : "", i, 10, frame[i], i % 2 ? "\n" : "\t");
		strcat_s(msg, buf);
	}

	MessageBoxA(hWnd, msg, title, MB_ICONERROR);
	exit(-1);
}