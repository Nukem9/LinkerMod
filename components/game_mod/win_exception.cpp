#include "stdafx.h"
#include <DbgHelp.h>

static_assert(EXCEPTION_STACKTRACE_MAXFRAMECOUNT < 63, "EXCEPTION_STACKTRACE_MAXFRAMECOUNT must be less than 63 to support Windows XP");

char g_ExceptionStr[EXCEPTION_STR_MAXLEN];

LONG WINAPI PrivateUnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	const char* localizedErr = Win_LocalizeRef("WIN_ERROR");

#if GM_USE_VERBOSE_EXCEPTION
	sprintf_s(g_ExceptionStr, "%s\nException: %s\n",
		localizedErr,
		ExceptionCodeToString(ExceptionInfo->ExceptionRecord->ExceptionCode));

	char buf[256];
	sprintf_s(buf, "Exception Address: %p\n\n", ExceptionInfo->ExceptionRecord->ExceptionAddress);
	strcat_s(g_ExceptionStr, buf);
	
	if (ExceptionInfo->ContextRecord->ContextFlags & CONTEXT_CONTROL)
	{
		sprintf_s(buf, "EBP: %08x\tEIP: %08x\nESP: %08x\tEFLAGS: %08x\n\nSEGCS: %08x\tSEGSS: %08x\t\n",
			ExceptionInfo->ContextRecord->Ebp,
			ExceptionInfo->ContextRecord->Eip,
			ExceptionInfo->ContextRecord->Esp,
			ExceptionInfo->ContextRecord->EFlags,
			ExceptionInfo->ContextRecord->SegCs,
			ExceptionInfo->ContextRecord->SegSs);
		strcat_s(g_ExceptionStr, buf);
	}

	if (ExceptionInfo->ContextRecord->ContextFlags & CONTEXT_SEGMENTS)
	{
		sprintf_s(buf, "SEGGS: %08x\tSEGFS: %08x\nSEGES: %08x\tSEGDS: %08x\n",
			ExceptionInfo->ContextRecord->SegGs,
			ExceptionInfo->ContextRecord->SegFs,
			ExceptionInfo->ContextRecord->SegEs,
			ExceptionInfo->ContextRecord->SegDs);
		strcat_s(g_ExceptionStr, buf);
	}

	if (ExceptionInfo->ContextRecord->ContextFlags & (CONTEXT_CONTROL | CONTEXT_SEGMENTS))
		strcat_s(g_ExceptionStr, "\n");

	if (ExceptionInfo->ContextRecord->ContextFlags & CONTEXT_INTEGER)
	{
		sprintf_s(buf, "EDI: %08x\tESI: %08x\nEAX: %08x\tEBX: %08x\nECX: %08x\tEDX: %08x\n\n",
			ExceptionInfo->ContextRecord->Edi,
			ExceptionInfo->ContextRecord->Esi,
			ExceptionInfo->ContextRecord->Eax,
			ExceptionInfo->ContextRecord->Ebx,
			ExceptionInfo->ContextRecord->Ecx,
			ExceptionInfo->ContextRecord->Edx);
		strcat_s(g_ExceptionStr, buf);
	}

	HMODULE dbgHelp = LoadLibraryA("dbghelp.dll");

	if (dbgHelp)
	{
		auto pStackWalk64			= (decltype(&StackWalk64))GetProcAddress(dbgHelp, "StackWalk64");
		auto pFunctionTableAccess	= (decltype(&SymFunctionTableAccess64))GetProcAddress(dbgHelp, "SymFunctionTableAccess64");
		auto pGetModuleBase			= (decltype(&SymGetModuleBase64))GetProcAddress(dbgHelp, "SymGetModuleBase64");

		if (pStackWalk64 && pFunctionTableAccess && pGetModuleBase)
		{
			strcat_s(g_ExceptionStr, "Stack Trace:\n");

			STACKFRAME64 frame;
			memset(&frame, 0, sizeof(STACKFRAME64));

			frame.AddrPC.Offset = ExceptionInfo->ContextRecord->Eip;
			frame.AddrPC.Mode = AddrModeFlat;
			frame.AddrFrame.Offset = ExceptionInfo->ContextRecord->Ebp;
			frame.AddrFrame.Mode = AddrModeFlat;
			frame.AddrStack.Offset = ExceptionInfo->ContextRecord->Esp;
			frame.AddrStack.Mode = AddrModeFlat;

			for (int i = 0; i < EXCEPTION_STACKTRACE_MAXFRAMECOUNT; i++)
			{
				if (!pStackWalk64(
					IMAGE_FILE_MACHINE_I386,
					GetCurrentProcess(),
					GetCurrentThread(),
					&frame,
					ExceptionInfo->ContextRecord,
					nullptr,
					pFunctionTableAccess,
					pGetModuleBase,
					nullptr))
				{
					// Failed or no frames left
					break;
				}

				if (frame.AddrPC.Offset != 0)
				{
					// Valid frame
					sprintf_s(buf, "frame[%02d]: %08llX %08llX %08llX %08llX\n",
						i,
						frame.AddrPC.Offset - sizeof(PVOID),
						frame.AddrReturn.Offset,
						frame.AddrFrame.Offset,
						frame.AddrStack.Offset);

					strcat_s(g_ExceptionStr, buf);
				}
				else
				{
					// Base reached
					break;
				}
			}
		}
	}
	
#else
	sprintf_s(g_ExceptionStr, "%s", localizedErr);
#endif

	Com_Error(ERR_FATAL, g_ExceptionStr);
	return EXCEPTION_EXECUTE_HANDLER;
}

const char *ExceptionCodeToString(DWORD ExceptionCode)
{
	switch (ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:			return "EXCEPTION_ACCESS_VIOLATION";
	case EXCEPTION_DATATYPE_MISALIGNMENT:		return "EXCEPTION_DATATYPE_MISALIGNMENT";
	case EXCEPTION_BREAKPOINT:					return "EXCEPTION_BREAKPOINT";
	case EXCEPTION_SINGLE_STEP:					return "EXCEPTION_SINGLE_STEP";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:		return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_FLT_DENORMAL_OPERAND:		return "EXCEPTION_FLT_DENORMAL_OPERAND";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:			return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_FLT_INEXACT_RESULT:			return "EXCEPTION_FLT_INEXACT_RESULT";
	case EXCEPTION_FLT_INVALID_OPERATION:		return "EXCEPTION_FLT_INVALID_OPERATION";
	case EXCEPTION_FLT_OVERFLOW:				return "EXCEPTION_FLT_OVERFLOW";
	case EXCEPTION_FLT_STACK_CHECK:				return "EXCEPTION_FLT_STACK_CHECK";
	case EXCEPTION_FLT_UNDERFLOW:				return "EXCEPTION_FLT_UNDERFLOW";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:			return "EXCEPTION_INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INT_OVERFLOW:				return "EXCEPTION_INT_OVERFLOW";
	case EXCEPTION_PRIV_INSTRUCTION:			return "EXCEPTION_PRIV_INSTRUCTION";
	case EXCEPTION_IN_PAGE_ERROR:				return "EXCEPTION_IN_PAGE_ERROR";
	case EXCEPTION_ILLEGAL_INSTRUCTION:			return "EXCEPTION_ILLEGAL_INSTRUCTION";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:	return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
	case EXCEPTION_STACK_OVERFLOW:				return "EXCEPTION_STACK_OVERFLOW";
	case EXCEPTION_INVALID_DISPOSITION:			return "EXCEPTION_INVALID_DISPOSITION";
	case EXCEPTION_GUARD_PAGE:					return "EXCEPTION_GUARD_PAGE";
	case EXCEPTION_INVALID_HANDLE:				return "EXCEPTION_INVALID_HANDLE";
	//case EXCEPTION_POSSIBLE_DEADLOCK:			return "EXCEPTION_POSSIBLE_DEADLOCK";
	}

	return "UNKNOWN";
}