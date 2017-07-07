#include "stdafx.h"

void __declspec(naked) hk_Scr_ReadFile()
{
	_asm
	{
		push[esp+16]	// archive
		push[esp+16]	// codePos
		push esi		// extFilename
		push[esp+20]	// filename
		push[esp+20]	// inst

		call Scr_ReadFile
		add esp, 20
		
		retn
	}
}

// /clientscript/cscr_parser.cpp:457
OpcodeLookup *Scr_GetPrevSourcePosOpcodeLookup(scriptInstance_t inst, const char *codePos)
{
	ASSERT(Scr_IsInOpcodeMemory(inst, codePos));
	ASSERT(gScrParserGlob[inst].opcodeLookup);

	unsigned int low = 0;
	unsigned int high = gScrParserGlob[inst].opcodeLookupLen - 1;

	// Binary search for the opcode->source map
	while (low <= high)
	{
		unsigned int middle = (high + low) / 2;

		if (codePos < gScrParserGlob[inst].opcodeLookup[middle].codePos)
		{
			high = middle - 1;
		}
		else
		{
			low = middle + 1;
			if (middle + 1 == gScrParserGlob[inst].opcodeLookupLen
				|| codePos < gScrParserGlob[inst].opcodeLookup[low].codePos)
			{
				return &gScrParserGlob[inst].opcodeLookup[middle];
			}
		}
	}

	ASSERT_MSG(false, "Unreachable");
	return nullptr;
}

// /clientscript/cscr_parser.cpp:548
unsigned int Scr_GetPrevSourcePos(scriptInstance_t inst, const char *codePos, unsigned int index)
{
	return gScrParserGlob[inst].sourcePosLookup[index + Scr_GetPrevSourcePosOpcodeLookup(inst, codePos)->sourcePosIndex].sourcePos;
}

// /clientscript/cscr_parser.cpp:594
int Scr_GetLineNumInternal(const char *buf, unsigned int sourcePos, const char **startLine, unsigned int *col, SourceBufferInfo *binfo)
{
	ASSERT(buf);

	*startLine = buf;
	int lineNum = 0;

	while (sourcePos)
	{
		if (!*buf)
		{
			*startLine = buf + 1;
			++lineNum;
		}

		++buf;
		--sourcePos;
	}

	*col = buf - *startLine;
	return lineNum;
}

// /clientscript/cscr_parser.cpp:641
int Scr_GetLineNum(scriptInstance_t inst, unsigned int bufferIndex, unsigned int sourcePos)
{
	const char *startLine;
	unsigned int col;

	ASSERT(gScrVarPub[inst].developer);

	return Scr_GetLineNumInternal(gScrParserPub[inst].sourceBufferLookup[bufferIndex].sourceBuf, sourcePos, &startLine, &col, nullptr);
}

// /clientscript/cscr_parser.cpp:932
char *Scr_ReadFile(scriptInstance_t inst, const char *filename, const char *extFilename, const char *codePos, bool archive)
{
	Scr_ReadFile_t func = nullptr;

	if (!scr_useFastFileOnly->current.integer && (fs_gameDirVar && *fs_gameDirVar->current.string || developer->current.enabled))
	{
		int file = 0;
		if (FS_FOpenFileRead(extFilename, &file) < 0)
		{
			func = Scr_ReadFile_FastFile;
		}
		else
		{
			FS_FCloseFile(file);
			func = Scr_ReadFile_LoadObj;
		}
	}
	else
	{
		func = Scr_ReadFile_FastFile;

		if (!useFastFile->current.enabled)
			func = Scr_ReadFile_LoadObj;
	}

	return func(inst, filename, extFilename, codePos, archive);
}

// /clientscript/cscr_parser.cpp:1055
void Scr_CopyFormattedLine(char *line, const char *rawLine)
{
	int len = strlen(rawLine);

	if (len >= 1024)
		len = 1023;

	// Replace tabs with spaces
	for (int i = 0; i < len; i++)
		line[i] = (rawLine[i] == '\t') ? ' ' : rawLine[i];

	// Terminate string early at newline
	if (line[len - 1] == '\r')
		line[len - 1] = '\0';

	line[len] = '\0';
}

// /clientscript/cscr_parser.cpp:1077
int Scr_GetLineInfo(const char *buf, unsigned int sourcePos, unsigned int *col, char *line, SourceBufferInfo *binfo)
{
	const char *startLine = nullptr;
	int lineNum = 0;

	if (buf)
	{
		lineNum = Scr_GetLineNumInternal(buf, sourcePos, &startLine, col, binfo);
	}
	else
	{
		lineNum = 0;
		startLine = "";
		*col = 0;
	}

	Scr_CopyFormattedLine(line, startLine);
	return lineNum;
}

// /clientscript/cscr_parser.cpp:1130
void Scr_PrintSourcePos(scriptInstance_t inst, int channel, const char *filename, const char *buf, unsigned int sourcePos)
{
	ASSERT(filename);

	char *type = "";
	char line[1024];
	unsigned int col;
	unsigned int lineNum = Scr_GetLineInfo(buf, sourcePos, &col, line, 0);

	if (gScrParserGlob[inst].saveSourceBufferLookup)
		type = " (savegame)";

	Com_PrintMessage(channel, va("(file '%s'%s, line %d)\n", filename, type, lineNum + 1), 0);
	Com_PrintMessage(channel, va("%s\n", line), 0);

	for (unsigned int i = 0; i < col; i++)
		Com_PrintMessage(channel, " ", 0);

	Com_PrintMessage(channel, "*\n", 0);
}

// /clientscript/cscr_parser.cpp:1199
unsigned int Scr_GetSourceBuffer(scriptInstance_t inst, const char *codePos)
{
	ASSERT(Scr_IsInOpcodeMemory(inst, codePos));
	ASSERT(gScrParserPub[inst].sourceBufferLookupLen > 0);

	unsigned int bufferIndex = gScrParserPub[inst].sourceBufferLookupLen - 1;

	for (; bufferIndex
		&& (!gScrParserPub[inst].sourceBufferLookup[bufferIndex].codePos
			|| gScrParserPub[inst].sourceBufferLookup[bufferIndex].codePos > codePos);
		--bufferIndex)
	{
		/* Do nothing */;
	}

	return bufferIndex;
}

// /clientscript/cscr_parser.cpp:1277
void Scr_PrintPrevCodePos(scriptInstance_t inst, int channel, const char *codePos, unsigned int index)
{
	// Frozen threads don't execute any code
	if (!codePos)
	{
		Com_PrintMessage(channel, "<frozen thread>\n", 0);
		return;
	}

	// Dead threads are at the "end" code marker
	if (codePos == (char *)0x032D6288)
	{
		Com_PrintMessage(channel, "<removed thread>\n", 0);
		return;
	}

	// If developer is enabled, we have source line information
	if (gScrVarPub[inst].developer)
	{
		if (gScrVarPub[inst].programBuffer && Scr_IsInOpcodeMemory(inst, codePos))
		{
			unsigned int bufferIndex = Scr_GetSourceBuffer(inst, codePos - 1);
			Scr_PrintSourcePos(
				inst,
				channel,
				gScrParserPub[inst].sourceBufferLookup[bufferIndex].buf,
				gScrParserPub[inst].sourceBufferLookup[bufferIndex].sourceBuf,
				Scr_GetPrevSourcePos(inst, codePos - 1, index));
			return;
		}
	}
	else if (Scr_IsInOpcodeMemory(inst, codePos - 1))
	{
		// Otherwise we just print the opcode index in memory
		Com_PrintMessage(channel, va("@ %d\n", codePos - gScrVarPub[inst].programBuffer), 0);
		return;
	}

	Com_PrintMessage(channel, va("%s\n\n", codePos), 0);
}

// /clientscript/cscr_parser.cpp:1923
void RuntimeErrorInternal(scriptInstance_t inst, int channel, const char *codePos, unsigned int index, const char *msg)
{
	ASSERT(Scr_IsInOpcodeMemory(inst, codePos));

	Com_PrintError(channel, "\n******* Script runtime error *******\n%s: ", msg);
	Scr_PrintPrevCodePos(inst, channel, codePos, index);

	if (gScrVmPub[inst].function_count > 0)
	{
		// Unwind the callstack, output to console
		for (int i = gScrVmPub[inst].function_count - 1; i >= 1; i--)
		{
			Com_PrintError(channel, "called from:\n");
			Scr_PrintPrevCodePos(
				inst,
				0,
				gScrVmPub[inst].function_frame_start[i].fs.pos,
				gScrVmPub[inst].function_frame_start[i].fs.localId == 0);
		}

		Com_PrintError(channel, "started from:\n");
		Scr_PrintPrevCodePos(inst, 0, gScrVmPub[inst].function_frame_start[0].fs.pos, 1);
	}

	Com_PrintError(channel, "************************************\n");
}

// /clientscript/cscr_parser.cpp:1957
void RuntimeError(scriptInstance_t inst, const char *codePos, unsigned int index, const char *msg, const char *dialogMessage)
{
	if (!gScrVarPub[inst].developer && !gScrVmPub[inst].terminal_error)
		return;

	ASSERT(Scr_IsInOpcodeMemory(inst, codePos));

	if (gScrVmPub[inst].debugCode)
	{
		Com_Printf(24, "%s\n", msg);

		if (!gScrVmPub[inst].terminal_error)
			return;

		goto error;
	}

	// Determine if user needs message box notification
	bool abort_on_error = gScrVmPub[inst].abort_on_error || gScrVmPub[inst].terminal_error;

#if SCRIPT_DEBUGGER
	if (Scr_IgnoreErrors(inst) && inst != SCRIPTINSTANCE_CLIENT)
		abort_on_error = false;
#endif

	// Dump callstack
	RuntimeErrorInternal(inst, abort_on_error != 0 ? 24 : 6, codePos, index, msg);

	if (abort_on_error)
	{
	error:
#if SCRIPT_DEBUGGER
		if (gScrVmPub[inst].terminal_error)
			Scr_IgnoreLeaks(inst);
#endif

		// This dvar forcefully disables all script runtime errors
		if (scr_suppressErrors->current.enabled)
			return;

		unsigned int bufferIndex = Scr_GetSourceBuffer(inst, codePos - 1);
		unsigned int sourcePos = Scr_GetPrevSourcePos(inst, codePos - 1, index);

		char line[1024];
		unsigned int col;
		unsigned int lineNum = Scr_GetLineInfo(gScrParserPub[inst].sourceBufferLookup[bufferIndex].sourceBuf, sourcePos, &col, line, 0);

		Com_Error(
			(gScrVmPub[inst].terminal_error) ? ERR_SCRIPT_DROP : ERR_SCRIPT,
			"%s script runtime error\n%s%s%s\n%s(%d):\n%s",
			(inst == SCRIPTINSTANCE_CLIENT) ? "Client" : "Server",
			msg,
			(dialogMessage) ? "\n" : "",
			(dialogMessage) ? dialogMessage : "",
			gScrParserPub[inst].sourceBufferLookup[bufferIndex].buf,
			lineNum,
			line);
	}
}