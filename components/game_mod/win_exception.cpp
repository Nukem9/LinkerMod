#include "win_exception.h"
#include "win_localize.h"
#include "common.h"

#include <iostream>

#define EXCEPTION_STR_MAXLEN 8192
#define EXCEPTION_STACKTRACE_MAXFRAMECOUNT 32

static_assert(EXCEPTION_STACKTRACE_MAXFRAMECOUNT < 63, "EXCEPTION_STACKTRACE_MAXFRAMECOUNT must be less than 63 to support Windows XP");

char g_ExceptionStr[EXCEPTION_STR_MAXLEN];

int __cdecl PrivateUnhandledExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo)
{
	const char* localizedErr = Win_LocalizeRef("WIN_ERROR");

#if GM_USE_VERBOSE_EXCEPTION
	sprintf_s(g_ExceptionStr, "%s\n%s", localizedErr, "Exception: ");

	DWORD errCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	if (errCode > 0xC0000005)
	{
		switch (errCode + 0x3FFFFFFA)
		{
		case 0x86:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n");
			break;
		case 0x87:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_DENORMAL_OPERAND\n");
			break;
		case 0x88:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_DIVIDE_BY_ZERO\n");
			break;
		case 0x89:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_INEXACT_RESULT\n");
			break;
		case 0x8A:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_INVALID_OPERATION\n");
			break;
		case 0x8B:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_OVERFLOW\n");
			break;
		case 0x8C:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_STACK_CHECK\n");
			break;
		case 0x8D:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_FLT_UNDERFLOW\n");
			break;
		case 0x17:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_ILLEGAL_INSTRUCTION\n");
			break;
		case 0x0:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_IN_PAGE_ERROR\n");
			break;
		case 0x8E:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_INT_DIVIDE_BY_ZERO\n");
			break;
		case 0x8F:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_INT_OVERFLOW\n");
			break;
		case 0x20:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_INVALID_DISPOSITION\n");
			break;
		case 0x1F:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_NONCONTINUABLE_EXCEPTION\n");
			break;
		case 0x90:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_PRIV_INSTRUCTION\n");
			break;
		case 0xF7:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_STACK_OVERFLOW\n");
			break;
		default:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "UNKNOWN\n");
			break;
		}
	}
	else
	{
		switch (errCode)
		{
		case 0xC0000005:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_ACCESS_VIOLATION\n");
			break;
		case 0x80000002:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_DATATYPE_MISALIGNMENT\n");
			break;
		case 0x80000003:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_BREAKPOINT\n");
			break;
		case 0x80000004:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "EXCEPTION_SINGLE_STEP\n");
			break;
		default:
			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "UNKNOWN\n");
			break;
		}
	}

	char buf[256];
	sprintf_s( buf, "Exception Address: %08x\n\n", ExceptionInfo->ExceptionRecord->ExceptionAddress);
	strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
	
	if (ExceptionInfo->ContextRecord->ContextFlags & 0x10001)
	{
		sprintf_s(buf, "EBP: %08x\tEIP: %08x\nESP: %08x\tEFLAGS: %08x\n\nSEGCS: %08x\tSEGSS: %08x\t\n",
			ExceptionInfo->ContextRecord->Ebp,
			ExceptionInfo->ContextRecord->Eip,
			ExceptionInfo->ContextRecord->Esp,
			ExceptionInfo->ContextRecord->EFlags,
			ExceptionInfo->ContextRecord->SegCs,
			ExceptionInfo->ContextRecord->SegSs);
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
	}

	if (ExceptionInfo->ContextRecord->ContextFlags & 0x10004)
	{
		sprintf_s(buf, "SEGGS: %08x\tSEGFS: %08x\nSEGES: %08x\tSEGDS: %08x\n",
			ExceptionInfo->ContextRecord->SegGs,
			ExceptionInfo->ContextRecord->SegFs,
			ExceptionInfo->ContextRecord->SegEs,
			ExceptionInfo->ContextRecord->SegDs);
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
	}

	if (ExceptionInfo->ContextRecord->ContextFlags & (0x10001 | 0x10004))
	{
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "\n");
	}

	if (ExceptionInfo->ContextRecord->ContextFlags & 0x10002)
	{
		sprintf_s(buf, "EDI: %08x\tESI: %08x\nEAX: %08x\tEBX: %08x\nECX: %08x\tEDX: %08x\n\n",
			ExceptionInfo->ContextRecord->Edi,
			ExceptionInfo->ContextRecord->Esi,
			ExceptionInfo->ContextRecord->Eax,
			ExceptionInfo->ContextRecord->Ebx,
			ExceptionInfo->ContextRecord->Ecx,
			ExceptionInfo->ContextRecord->Edx);
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
	}

	//
	// Print the stack context bytes (disabled due to overflow issues)
	//
	/*if (ExceptionInfo->ContextRecord->ContextFlags & 0x10001)
	{
		strcat_s( g_ExceptionStr, EXCEPTION_STR_MAXLEN, "Stack Bytes: \n");

		for (int i = 0; i < 128; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				sprintf_s(buf, "%02x ", *(BYTE*)(ExceptionInfo->ContextRecord->Esp + j + 8 * i));
				strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
			}

			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "\t[");

			for (int j = 0; j < 8; ++j)
			{
				if ((signed int)*(BYTE*)(ExceptionInfo->ContextRecord->Esp + j + 8 * i) >= 33)
				{
					sprintf_s(buf, "%c", *(BYTE*)(ExceptionInfo->ContextRecord->Esp + j + 8 * i));
					strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
				}
				else
				{
					sprintf_s(buf, ".");
					strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
				}
			}

			strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "]\n");
		}
		
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "\n");
	}*/
	
	void* frame[EXCEPTION_STACKTRACE_MAXFRAMECOUNT];
	int frameCount = CaptureStackBackTrace(0, EXCEPTION_STACKTRACE_MAXFRAMECOUNT, frame, NULL);

	if (frameCount != 0)
	{
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, "Stack Trace:\n");
	}

	for (int i = 0; i < frameCount; i++)
	{
		sprintf_s(buf, "frame[%s%d]: %08x%s", i < 10 ? " " : "", i, frame[i], i % 2 ? "\n" : "\t");
		strcat_s(g_ExceptionStr, EXCEPTION_STR_MAXLEN, buf);
	}

#else
	sprintf_s(g_ExceptionStr, "%s", localizedErr);
#endif

	Com_Error(0, g_ExceptionStr);
	return 1;
}
