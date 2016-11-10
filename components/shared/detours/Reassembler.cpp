#include "stdafx.h"

namespace Reassembler
{
	// This converts a distorm decoded instruction into text format
	// XEDParse then re-assembles it in order to account for the IP changing
	// Not all instructions will work (2 or 4GB boundary)
	BYTE *Reassemble(uint64_t Ip, _DecodedInst *Instructions, int Count, size_t *LengthOut, AsmGen *Gen)
	{
		for (int i = 0; i < Count; i++)
		{
			_DecodedInst *instr = &Instructions[i];

			Gen->AddCode("%s %s", instr->mnemonic.p, instr->operands.p);
		}

		*LengthOut = Gen->GetStreamLength();
		return Gen->GetStream(false);
	}

	BYTE *Reassemble32(uint32_t Ip, _DecodedInst *Instructions, int Count, size_t *LengthOut)
	{
		AsmGen gen(Ip, ASMGEN_32);
		return Reassemble(Ip, Instructions, Count, LengthOut, &gen);
	}

	BYTE *Reassemble64(uint64_t Ip, _DecodedInst *Instructions, int Count, size_t *LengthOut)
	{
		AsmGen gen(Ip, ASMGEN_64);
		return Reassemble(Ip, Instructions, Count, LengthOut, &gen);
	}

	void Free(BYTE *Data)
	{
		free(Data);
	}
}