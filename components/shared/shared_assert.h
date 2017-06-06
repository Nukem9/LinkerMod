#pragma once

#include <iostream>
#include <Windows.h>

#define ASSERT(cond)					if(!(cond)) shared_assert(__FILE__, __LINE__, #cond);
#define ASSERT_MSG_VA(cond, msg, ...)	if(!(cond)) shared_assert(__FILE__, __LINE__, "%s\n\n" msg, #cond, ##__VA_ARGS__);
#define ASSERT_MSG(cond, msg)			ASSERT_MSG_VA(cond, msg);

#define STATIC_ASSERT_SIZE(struct, size)				static_assert(sizeof(struct) == size, "Size check")
#define STATIC_ASSERT_OFFSET(struct, member, offset)	static_assert(offsetof(struct, member) == offset, "Offset check")

#if _DEBUG
	#define DBG_ASSERT(cond)			ASSERT(cond)
	#define DBG_ASSERT_MSG(cond, msg)	ASSERT_MSG(cond, msg)
#else
	#define DBG_ASSERT(cond)
	#define DBG_ASSERT_MSG(cond, msg)
#endif

static void shared_assert(const char *File, int Line, const char *Format, ...)
{
#if RADIANT_MOD
	return;
#endif
	char buffer[4096];
	char message[4096];

	va_list ap;
	va_start(ap, Format);

	_vsnprintf_s(buffer, _TRUNCATE, Format, ap);
	sprintf_s(message, "%s(%d):\n\n%s", File, Line, buffer);

	MessageBoxA(nullptr, message, "ASSERTION", MB_ICONERROR);

	if (IsDebuggerPresent())
		__debugbreak();

	ExitProcess(1);
}