#pragma once

#include "stdafx.h"

enum AsmGenType
{
	ASMGEN_32,
	ASMGEN_64,
};

class AsmGen
{
public:

private:
	ULONGLONG			m_Ip;
	bool				m_X64Code;
	std::vector<BYTE>	m_Stream;
	BYTE				*m_StreamData;

public:
	AsmGen(BYTE *Ip, AsmGenType Type) : AsmGen((ULONGLONG)Ip, Type) {}
	AsmGen(ULONGLONG Ip, AsmGenType Type);
	~AsmGen();

	bool AddCode(char *Instruction);
	bool AddCode(const char *Format, ...);
	bool AddCodeArray(const char **Instructions, size_t Count);

	BYTE *GetStream(bool Free = true);
	size_t GetStreamLength();
};