#pragma once

namespace Reassembler
{
	BYTE *Reassemble32(uint32_t Ip, _DecodedInst *Instructions, int Count, size_t *LengthOut);
	BYTE *Reassemble64(uint64_t Ip, _DecodedInst *Instructions, int Count, size_t *LengthOut);
	void Free(BYTE *Data);
}