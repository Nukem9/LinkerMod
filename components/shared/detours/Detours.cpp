#include "stdafx.h"

namespace Detours
{
	uint32_t GlobalOptions;

	void SetGlobalOptions(uint32_t Options)
	{
		InterlockedExchange((volatile LONG *)&GlobalOptions, Options & OPT_MASK);
	}

	uint32_t GetGlobalOptions()
	{
		return GlobalOptions;
	}

	uint8_t *DetourAlignAddress(uint64_t Address, uint8_t Align)
	{
		if (Address % Align != 0)
			Address += Align - Address % 8;

		return (uint8_t *)Address;
	}

	bool DetourAtomicCopy4X8(uint8_t *Target, uint8_t *Memory, sizeptr_t Length)
	{
		// Buffer to hold temporary opcodes
		char buffer[8];

		// Only 4/8byte sizes supported
		if(Length > sizeof(buffer))
			return false;

		DWORD dwOld = 0;
		if (!VirtualProtect(Target, Length, PAGE_EXECUTE_READWRITE, &dwOld))
			return false;

		if(Length <= 4)
		{
			// Copy the old data
			memcpy(&buffer, Target, 4);

			// Rewrite it with the new data
			memcpy(&buffer, Memory, Length);

			// Write all 4 bytes at once
			InterlockedExchange((volatile LONG *)Target, *(LONG *)&buffer);
		}
		else if(Length <= 8)
		{
			// Copy the old data
			memcpy(&buffer, Target, 8);

			// Rewrite it with the new data
			memcpy(&buffer, Memory, Length);

			// Write all 8 bytes at once
			_intrinInterlockedExchange64((volatile LONGLONG *)Target, *(LONGLONG *)&buffer);
		}

		// Ignore if this fails, the memory was copied either way
		VirtualProtect(Target, Length, dwOld, &dwOld);

		return true;
	}

	bool DetourCopyMemory(uint8_t *Target, uint8_t *Memory, sizeptr_t Length)
	{
		DWORD dwOld = 0;
		if (!VirtualProtect(Target, Length, PAGE_EXECUTE_READWRITE, &dwOld))
			return false;

		memcpy(Target, Memory, Length);

		// Ignore if this fails, the memory was copied either way
		VirtualProtect(Target, Length, dwOld, &dwOld);

		return true;
	}

	bool DetourFlushCache(uint8_t *Target, sizeptr_t Length)
	{
		return FlushInstructionCache(GetCurrentProcess(), Target, Length) != FALSE;
	}
}